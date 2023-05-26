#include "Event.hpp"
#include "EventLoop.hpp"
#include "EventManager.hpp"
#include <sys/_types/_socklen_t.h>
#include <sys/_types/_ssize_t.h>
#include <sys/socket.h>

void EventLoop::readCallback(struct kevent *e)
{
	std::cout << "\033[32m"; 
	std::cout<<"Read callback"<<std::endl;

	//set event
	Event *e_udata = static_cast<Event *>(e->udata);

	switch (e_udata->getEventType()){
		//check us server socket
		case E_SERVER_SOCKET:
			e_serverSocketCallback(e, e_udata);
			break;
		//check is client socket read
		case E_CLIENT_SOCKET:
			e_clientSocketCallback(e, e_udata);
			break;
		//check is pipe read
		case E_PIPE:
			e_pipeCallback(e, e_udata);
			break;
		//check is file read
		case E_FILE:
			e_fileCallback(e, e_udata);
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
void EventLoop::e_serverSocketCallback(struct kevent *e, Event *e_udata)
{
	std::cout << "\033[33m"; 
	std::cout<<"SERVER SOCKET CALLBACK"<<std::endl;
	//we need to verify http
	if (e_udata->getServerType() == HTTP_SERVER)
	{
		//create client socket
		//모든 pipe, file의 이벤트는 client socket에서 부터 시작한다.
		Event *new_udata = Event::createNewClientSocketEvent(e_udata);
		new_udata->setRequestHandler(new HttpreqHandler());
		registerClientSocketReadEvent(new_udata);
	}
}

void EventLoop::e_clientSocketCallback(struct kevent *e, Event *e_udata)
{
	std::cout << "\033[34m"; 
	std::cout<<"client socket callback"<<std::endl;
	//socket의 readfilter-> EOF flag는 client의 disconnect.
	if (e->flags == EV_EOF)
	{
		EventManager::getInstance().deleteEvent(e->ident);
		close(e->ident);
		std::cout<<"client disconnected"<<std::endl;
	}

	//Http Server인 소켓에서 연결된 client_fd에 대한 read처리
	if (e_udata->getServerType() == HTTP_SERVER)
	{
		//read from client socket
		int client_fd = e_udata->getClientFd();
		ssize_t read_len = read(client_fd, HttpServer::getInstance().getHttpBuffer(), 1024);
		HttpServer::getInstance().getStringBuffer().insert(0, HttpServer::getInstance().getHttpBuffer(), read_len);
		if (read_len == -1)
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK)
				return;
			else
				throw std::runtime_error("Failed to read from client socket, unknown err\n");
		}
		else
		{
			std::cout<<"[[[[[[[CLIENT REQUEST START]]]]]]]]"<<std::endl;
			std::cout<<HttpServer::getInstance().getHttpBuffer()<<std::endl;
			std::cout<<"[[[[[[[CLIENT REQUEST END]]]]]]]]"<<std::endl;

			HttpreqHandler *reqHandler = static_cast<HttpreqHandler *>(e_udata->getRequestHandler());

			//handle request
			reqHandler->handle(&(HttpServer::getInstance().getStringBuffer()));
			reqHandler->printReq();
			//handle response by request
			
			/**
			 * pending state => client로부터 data를 더 받아야하는 상태
			 * */
			if (reqHandler->getIsPending())
				return ;
			else
			{
				/**
				 * response를 보내야하는 상태임.
				 * */
				/**
				 * if need file i/o
				 * */
				/**
				 * else if need cgi(pipe)
				 * */
			}
		}
	}
}

/* Fifos, Pipes */
/* Returns when there is data to read; data contains the number of bytes available. */
/* When the last writer disconnects, the filter will set EV_EOF in flags. */
/* This may be cleared by passing in EV_CLEAR, at which point the filter will */
/* resume waiting for data to become available before returning. */
void EventLoop::e_pipeCallback(struct kevent *e, Event *e_udata)
{
	std::cout << "\033[35m"; 
	std::cout<<"pipe callback"<<std::endl;
	if (e_udata->getServerType() == HTTP_SERVER)
	{
		//read from pipe
	}
}

void EventLoop::e_fileCallback(struct kevent *e, Event *e_udata)
{
	std::cout << "\033[36m"; 
	std::cout<<"file callback"<<std::endl;

}
