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
		Event *new_udata = new Event(HTTP_SERVER);

		EventManager::getInstance().addEvent(new_udata);
		t_SocketInfo socketInfo;
		int sockfd = e_udata->getServerFd();
		socklen_t cliLen = sizeof(socketInfo.socket_addr);
		int client_fd;


		client_fd = accept(sockfd, (struct sockaddr *)&(socketInfo.socket_addr), &cliLen);
		if (client_fd == -1)
		{
			delete new_udata;
			throw std::runtime_error("Failed to accept client socket\n");
		}

		/**
		 * event에 전달할 udata 채우기!
		 * */
		/* client socket에 대한 read event입니다.*/
		new_udata->setEventType(E_CLIENT_SOCKET);

		/**
		 * client socket의 fd를 등록
		 * */
		new_udata->setClientFd(client_fd);

		/**
		 * client socket의 socketInfo를 등록
		 * */
		new_udata->setSocketInfo(socketInfo);

		/**
		 * client socket에 Location data 등록
		 * 현재 udata의 Event에 이미 port별로 location data가 있음.
		 * 그대로 복사
		 * default server data는 생성자에서 초기화됨.
		 * */
		new_udata->setLocationData(e_udata->getLocationData());

		//client socket을 읽기전용으로  kqueue에 등록
		EV_SET(&(dummyEvent), client_fd, EVFILT_READ, EV_ADD, 0, 0, new_udata);
		if (kevent(this->kq_fd, &(dummyEvent), 1, NULL, 0, NULL) == -1) 
			throw std::runtime_error("Failed to register client socket with kqueue\n");

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

			e_udata->getRequestHandler()->handle(&(HttpServer::getInstance().getStringBuffer()));
			static_cast<HttpreqHandler *>(e_udata->getRequestHandler())->printReq();

			//handle response by request
		}
	}
}

void EventLoop::e_pipeCallback(struct kevent *e, Event *e_udata)
{
	std::cout << "\033[35m"; 
	std::cout<<"pipe callback"<<std::endl;

}

void EventLoop::e_fileCallback(struct kevent *e, Event *e_udata)
{
	std::cout << "\033[36m"; 
	std::cout<<"file callback"<<std::endl;

}
