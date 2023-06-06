#include "Event.hpp"
#include "EventLoop.hpp"
#include <exception>
#include <sys/errno.h>
#include <sys/event.h>
#include <unistd.h>

void EventLoop::readCallback(struct kevent *e)
{
	std::cout << "\033[32m"; 
	std::cout<<"Read callback"<<std::endl;
	Event *e_udata = static_cast<Event *>(e->udata);
	if (e_udata->r_serverEvent)
		e_serverSocketReadCallback(e, e_udata);
	if (e_udata->r_clientEvent)
		e_clientSocketReadCallback(e, e_udata);
	if (e_udata->r_pipeEvent)
		e_pipeReadCallback(e, e_udata);
	if (e_udata->r_fileEvent)
		e_fileReadCallback(e, e_udata);
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
	std::cout<<"Client socket read callback"<<std::endl;
	//Http Server인 소켓에서 연결된 client_fd에 대한 read처리
	if (e_udata->getServerType() == HTTP_SERVER)
	{
		/**
		 * EOF => client가 보내는 데이터가 끝났음을 의미한다.
		 * unregister이후, client에 socket write가 끝나면 다시 register한다.
		 * */
		if (e->flags & EV_EOF)
		{
			std::cout<<"EV_EOF"<<std::endl;
			unregisterClientSocketReadEvent(e_udata);
			return ;
		}

		HttpreqHandler *reqHandler = static_cast<HttpreqHandler *>(e_udata->getRequestHandler());
		//read from client socket
		int client_fd = e_udata->getClientFd();
		ssize_t read_len = read(client_fd, HttpServer::getInstance().getHttpBuffer(), HTTPBUFFER_SIZE - 1);
 
		std::cout<<"readd_len : "<<read_len<<std::endl;
		if (read_len == -1)
		{
			std::cout<<"errno:"<<errno<<std::endl;
			if (errno == EAGAIN || errno == EWOULDBLOCK)
				return;
			else if (errno == ECONNRESET)
			{
				std::cout<<"ECONNRESET"<<std::endl;
				close(client_fd);
				delete e_udata;
				return ;
			}
		}
		else if (read_len == 0)
		{
			std::cout<<"read len is 0"<<std::endl;
			return;
		}
		else
			{
			HttpServer::getInstance().getHttpBuffer()[read_len] = '\0';
		
			/* std::cout<<"Httpbuffer read"<<std::endl; */
			/* write(1, HttpServer::getInstance().getHttpBuffer(), read_len); */
			/* std::cout<<std::endl; */
			e_udata->readByte = read_len;

			//handle request
			try {
				/* std::cout<<"use handle"<<std::endl; */
				reqHandler->handle(e_udata);
				/* std::cout<<"use handle end"<<std::endl; */
			} catch (std::exception &exception) {
				std::cout<<"Error in handle"<<std::endl;
				errorCallback(e_udata);
				return;
			}
			//handle response by request
			/**
			 * pending state => client로부터 data를 더 받아야하는 상태
			 *
			 * read_len : read return 0 when eof
			 * */
			/* std::cout<<"header pendig::"<<reqHandler->isHeaderPending()<<std::endl; */
			/* std::cout<<"body pendig::"<<reqHandler->isBodyPending()<<std::endl; */
			/* std::cout<<"pending::"<<reqHandler->getIsPending()<<std::endl; */
			/* std::cout<<reqHandler->getRequestInfo().body<<std::endl; */
			if (reqHandler->isHeaderPending())
				return ;
			else
			{
				if (e_udata->locationData)
					return ;
				/* reqHandler->printReq(); */
				/**
				 * initialize internal method and uri
				 * */
				std::cerr<<"=======================initialize internal method and uri================="<<std::endl;
				std::cerr<<"method: "<<reqHandler->getRequestInfo().method<<std::endl;
				std::cerr<<"uri: "<<reqHandler->getRequestInfo().path<<std::endl;
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
		HttpreqHandler *reqHandler = static_cast<HttpreqHandler *>(e_udata->getRequestHandler());
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
			if (reqHandler->getIsPending())
				return ;
			else
			{
				unregisterPipeReadEvent(e_udata);
				registerClientSocketWriteEvent(e_udata);
				return;
			}
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
		{
			unregisterFileReadEvent(e_udata);
			registerClientSocketWriteEvent(e_udata);
			return ;
		}
		else
			{
			HttpServer::getInstance().getHttpBuffer()[read_len] = '\0';
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
