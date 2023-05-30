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
	std::cout << "\033[33m"; 
	std::cout<<"SERVER SOCKET CALLBACK"<<std::endl;
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
	std::cout<<"client socket callback"<<std::endl;
	//Http Server인 소켓에서 연결된 client_fd에 대한 read처리
	if (e_udata->getServerType() == HTTP_SERVER)
	{
		//socket의 readfilter-> EOF flag는 client의 disconnect.
		if (e->flags == EV_EOF)
		{
			unregisterClientSocketReadEvent(e_udata);
			//remove event
			std::cout<<"client disconnected"<<std::endl;
		}

		//read from client socket
		int client_fd = e_udata->getClientFd();
		ssize_t read_len = read(client_fd, HttpServer::getInstance().getHttpBuffer(), 1024);
		HttpServer::getInstance().getHttpBuffer()[read_len] = '\0';
		
		std::cout<<"read len = " <<read_len<<std::endl;

		if (read_len != -1)
			HttpServer::getInstance().getStringBuffer().insert(0, HttpServer::getInstance().getHttpBuffer(), read_len);
		e_udata->readByte = read_len;
		std::cout<<"data::"<< e->data<<std::endl;
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
				std::cout<<"client disconnected"<<std::endl;
				return;
			}
			else
				//관련 exception 처리 필요
				//event 삭제?
				throw std::runtime_error("Failed to read from client socket, unknown err\n");
		}
		else if (read_len == 0)
		{
			unregisterClientSocketReadEvent(e_udata);
			registerClientSocketWriteEvent(e_udata);
		}
		else
		{
			std::cout<<"[[[[[[[CLIENT REQUEST START]]]]]]]]"<<std::endl;
			std::cout<<HttpServer::getInstance().getHttpBuffer()<<std::endl;
			std::cout<<"[[[[[[[CLIENT REQUEST END]]]]]]]]"<<std::endl;

			HttpreqHandler *reqHandler = static_cast<HttpreqHandler *>(e_udata->getRequestHandler());

			//handle request
			e_udata->setBuffer(&HttpServer::getInstance().getStringBuffer());
			try {
				std::cout<<"use handle"<<std::endl;
				reqHandler->handle(e_udata);
				std::cout<<"use handle end"<<std::endl;
			} catch (std::exception &exception) {
				/**
				 * client request exception handling by 
				 * register write event to client_fd, and finally send error response
				 * */
				std::cout<<"catch some exception"<<std::endl;
				std::cout<<"statudcode"<<e_udata->getStatusCode()<<std::endl;
				unregisterClientSocketReadEvent(e_udata);
				registerClientSocketWriteEvent(e_udata);
				/**
				 * in here, the read event for this client_fd disabled
				 * and the write event to client socket add and enabled
				 * */
				return ;
			}



			//handle response by request
			/**
			 * pending state => client로부터 data를 더 받아야하는 상태
			 *
			 * read_len : read return 0 when eof
			 * */
			if (reqHandler->getIsPending() && read_len != 0)
			{
				std::cout<<"pending"<<std::endl;
				return ;
			}
			else
			{
				try {
					/**
					 * set http response
					 * */
					std::cout<<"setting response"<<std::endl;
					setHttpResponse(e_udata);
				} catch (std::exception &e) {
					/**
					 * http response가 설정 중간에 exception이 발생할 경우
					 * limited method => 405
					 * */
					std::cout<<"catch some exception in setting response"<<std::endl;
					std::cout<<"statudcode"<<e_udata->getStatusCode()<<std::endl;
					unregisterClientSocketReadEvent(e_udata);
					registerClientSocketWriteEvent(e_udata);
					return ;
				}
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
	std::cout << "\033[35m"; 
	std::cout<<"pipe read callback"<<std::endl;
	if (e_udata->getServerType() == HTTP_SERVER)
	{
		//eof flag있어야 writer가 disconnected한거
		if (e->flags == EV_EOF)
		{
			//pipe의 writer는 즉 cgi process임.
			//이제 이 fd는 close해도 됨.
			unregisterPipeReadEvent(e_udata);
			registerClientSocketWriteEvent(e_udata);
			e_udata->setEventType(E_CLIENT_SOCKET);
			return;
		}

		//read from pipe
		ssize_t read_len = read(e_udata->getPipeFd()[0], HttpServer::getInstance().getHttpBuffer(), 1024);
		HttpServer::getInstance().getHttpBuffer()[read_len] = '\0';
		std::cout<<"read len = " <<read_len<<std::endl;
		if (read_len == -1)
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK)
				return;
			else
				//관련 exception 처리 필요
				//event 삭제?
				throw std::runtime_error("Failed to read from pipe, unknown err\n");
		}
		else if (read_len == 0)
			{
				//pipe의 writer는 즉 cgi process임.
				//이제 이 fd는 close해도 됨.
				unregisterPipeReadEvent(e_udata);
				registerClientSocketWriteEvent(e_udata);
				e_udata->setEventType(E_CLIENT_SOCKET);
				return;
			}
		else
		{
			//read from pipe
			std::cout<<"[[[[[[[PIPE READ START]]]]]]]]"<<std::endl;
			std::cout<<HttpServer::getInstance().getHttpBuffer()<<std::endl;
			std::cout<<"[[[[[[[PIPE READ END]]]]]]]]"<<std::endl;


			/**
			 * response body에 읽은 데이터 추가.
			 * */
			HttpServer::getInstance().getStringBuffer().insert(0, HttpServer::getInstance().getHttpBuffer(), read_len);
			static_cast<responseHandler *>(e_udata->getResponseHandler())->setResBody(HttpServer::getInstance().getStringBuffer());
		}
	}
}

void EventLoop::e_fileReadCallback(struct kevent *e, Event *e_udata)
{
	std::cout << "\033[36m"; 
	std::cout<<"file callback"<<std::endl;
	if (e_udata->getServerType() == HTTP_SERVER)
	{
		//eof flag있어야 writer가 작성 끝낸거임.
		if (e->flags & EV_EOF)
		{
		}
	}
}
