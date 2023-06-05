#include "Event.hpp"
#include "EventLoop.hpp"
#include <exception>
#include <sys/event.h>

void EventLoop::readCallback(struct kevent *e)
{
	std::cout << "\033[32m"; 
	std::cout<<"Read callback"<<std::endl;

	//set event
	Event *e_udata = static_cast<Event *>(e->udata);

	switch (e_udata->getEventType()){
		//check us server socket
		case E_SERVER_SOCKET:
			e_serverSocketReadCallback(e, e_udata);
			break;
		//check is client socket read
		case E_CLIENT_SOCKET:
			e_clientSocketReadCallback(e, e_udata);
			break;
		//check is pipe read
		case E_PIPE:
			e_pipeReadCallback(e, e_udata);
			break;
		//check is file read
		case E_FILE:
			e_fileReadCallback(e, e_udata);
			break;
		default:
			std::cout<<"unknown event type"<<std::endl;
			break;
	}
}

/**
 * @brief serverSocket의 read event callback.
 *
 * server socket에서 발생한 read event에 대해 
 * client socket을 생성하고, client socket에 대한 read event를 등록한다.
 * @param e
 */
void EventLoop::e_serverSocketReadCallback(struct kevent *e, Event *e_udata)
{
	//we need to verify http
	if (e_udata->getServerType() == HTTP_SERVER)
	{
		//create client socket
		//모든 pipe, file의 이벤트는 client socket에서 부터 시작한다.
		Event *new_udata = Event::createNewClientSocketEvent(e_udata);

		//handler 객체 설정
		new_udata->setRequestHandler(new HttpreqHandler());
		new_udata->setResponseHandler(new responseHandler(15));

		//kqueue에 event 등록
		registerClientSocketReadEvent(new_udata);
	}
}

void EventLoop::e_clientSocketReadCallback(struct kevent *e, Event *e_udata)
{
	std::cout << "\033[34m"; 
	//Http Server인 소켓에서 연결된 client_fd에 대한 read처리
	if (e_udata->getServerType() == HTTP_SERVER)
	{
		//socket의 readfilter-> EOF flag는 client의 disconnect.
		// if (e->flags == EV_EOF)
		// {
		// 	unregisterClientSocketReadEvent(e_udata);
		// 	//remove event
		// 	//client socket close
		// 	close(e_udata->getClientFd());
		// 	//client socket event delete
		// 	delete e_udata;
		// }

		//read from client socket
		int client_fd = e_udata->getClientFd();
		ssize_t read_len = read(client_fd, HttpServer::getInstance().getHttpBuffer(), HTTPBUFFER_SIZE - 1);
  
		if (read_len == -1)
		{
			std::cout<<"errno:"<<errno<<std::endl;
			if (errno == EAGAIN || errno == EWOULDBLOCK)
				return;
			else if (errno == ECONNRESET)
			{
				//remove event
				unregisterClientSocketReadEvent(e_udata);
				//client socket close
				close(client_fd);
				//client socket event delete
				delete e_udata;
				return;
			}
			else
				//관련 exception 처리 필요
				//event 삭제?
				throw std::runtime_error("Failed to read from client socket, unknown err\n");
		}
		else if (read_len == 0)
			// unregisterClientSocketReadEvent(e_udata);
			return;
		else
			{
			HttpServer::getInstance().getHttpBuffer()[read_len] = '\0';
			e_udata->readByte = read_len;

			/* std::cout<<"[[[[[[[CLIENT REQUEST START]]]]]]]]"<<std::endl; */
			/* std::cout<<HttpServer::getInstance().getHttpBuffer()<<std::endl; */
			/* std::cout<<"[[[[[[[CLIENT REQUEST END]]]]]]]]"<<std::endl; */

			HttpreqHandler *reqHandler = static_cast<HttpreqHandler *>(e_udata->getRequestHandler());

			//handle request
			try {
				/* std::cout<<"use handle"<<std::endl; */
				reqHandler->handle(e_udata);
				/* std::cout<<"use handle end"<<std::endl; */
			} catch (std::exception &exception) {
				errorCallback(e_udata);
				return;
			}
			//handle response by request
			/**
			 * pending state => client로부터 data를 더 받아야하는 상태
			 *
			 * read_len : read return 0 when eof
			 * */
			if (reqHandler->isHeaderPending() && read_len != 0)
				return ;
			else
			{
				/**
				 * initialize internal method and uri
				 * */
				e_udata->internal_method = reqHandler->getRequestInfo().method;
				e_udata->internal_uri = reqHandler->getRequestInfo().path;
				setHttpResponse(e_udata);
			}
		}
	}
}

/**
* Fifos, Pipes
* Returns when there is data to read; data contains the number of bytes available.
* When the last writer disconnects, the filter will set EV_EOF in flags.
* This may be cleared by passing in EV_CLEAR, at which point the filter will
* resume waiting for data to become available before returning.
**/
void EventLoop::e_pipeReadCallback(struct kevent *e, Event *e_udata)
{
	std::cout<<"pipeWrite"<<std::endl;
	if (e_udata->getServerType() == HTTP_SERVER)
	{
		//read from pipe
		ssize_t read_len = read(e_udata->CtoPPipe[0], EventLoop::getInstance().pipeBuffer, 65534);
		/* std::cout<<"read len = " <<read_len<<std::endl; */
		/* std::cout<<"pipe readable data size:"<<e->data<<std::endl; */
		if (read_len == -1)
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK)
				return;
			else
				//관련 exception 처리 필요
				//event 삭제?
			{
				std::cout<<"pipe read error. read len is -1, errno: "<<errno<<std::endl;
				e_udata->setStatusCode(500);
				unregisterPipeReadEvent(e_udata);
				registerClientSocketWriteEvent(e_udata);
				return;
			}
		}
		else if (read_len == 0)
		{
			unregisterPipeReadEvent(e_udata);
			registerClientSocketWriteEvent(e_udata);
			return;
		}
		else
		{
			EventLoop::getInstance().pipeBuffer[read_len] = '\0';
			static_cast<responseHandler *>(e_udata->getResponseHandler())->setResBody(EventLoop::getInstance().pipeBuffer);
		}
	}
}

void EventLoop::e_fileReadCallback(struct kevent *e, Event *e_udata)
{
	if (e_udata->getServerType() == HTTP_SERVER)
	{
		//read from file
		ssize_t read_len = read(e_udata->file_fd, HttpServer::getInstance().getHttpBuffer(), HTTPBUFFER_SIZE - 1);
		std::cout<<read_len<<std::endl;

		if (read_len == -1)
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK)
				return ;
			else
				throw std::runtime_error("Failed to read from file, unknown err\n");
		}
		else if (read_len == 0)
		{unregisterFileReadEvent(e_udata);
			registerClientSocketWriteEvent(e_udata);
			return ;
		}
		else
			{
			HttpServer::getInstance().getHttpBuffer()[read_len - 1] = '\0';
			static_cast<responseHandler *>(e_udata->getResponseHandler())->setResBody(HttpServer::getInstance().getHttpBuffer());
			e_udata->fileReadByte += read_len;

			if (e_udata->fileReadByte == e_udata->statBuf.st_size)
			{
				unregisterFileReadEvent(e_udata);
				registerClientSocketWriteEvent(e_udata);
				return ;
			}
		}
	}
}
