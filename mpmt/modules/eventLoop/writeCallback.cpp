#include "EventLoop.hpp"
#include <sys/errno.h>

void EventLoop::writeCallback(struct kevent *e)
{
	std::cout << "\033[32m"; 
	std::cout<<"Read callback"<<std::endl;

	//set event
	Event *e_udata = static_cast<Event *>(e->udata);

	switch (e_udata->getEventType()){
		case E_CLIENT_SOCKET:
			e_clientSocketWriteCallback(e, e_udata);
			break;
		//check is pipe read
		case E_PIPE:
			e_pipeWriteCallback(e, e_udata);
			break;
		//check is file read
		case E_FILE:
			e_fileWriteCallback(e, e_udata);
			break;
		default:
			std::cout<<"unknown event type"<<std::endl;
			break;
	}
}

void EventLoop::e_clientSocketWriteCallback(struct kevent *e, Event *e_udata)
{
	std::cout << "\033[33m"; 
	std::cout<<"CLIENT SOCKET WRITE CALLBACK"<<std::endl;
	//we need to verify http
	if (e_udata->getServerType() == HTTP_SERVER)
	{
		//status check
		//make response message before write

		/**
		 * set response message
		 * */

		/**
		 * size of e->data만큼 작성
		 * */
		int wroteByte = write(e_udata->getClientFd(), NULL, e->data);

		if (wroteByte == -1)
		{
			if (errno == EWOULDBLOCK || errno == EAGAIN)
				return;
			else
			{
				//관련 exception 처리 필요
				throw std::runtime_error("Failed to read from client socket, unknown err\n");
			}
		}

		else
		{
			e_udata->wrote -= wroteByte; 
			if (e_udata->wrote == 0)
				//작성완료.
				unregisterClientSocketWriteEvent(e_udata);
		}
	}
}

void EventLoop::e_pipeWriteCallback(struct kevent *e, Event *e_udata){}
void EventLoop::e_fileWriteCallback(struct kevent *e, Event *e_udata){}
