#include "EventLoop.hpp"

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
		if (e_udata->getStatusCode() >= 400)
		{
			/**
			 * write buffer in response
			 * */
			/* write(e_udata->getClientFd(), e_udata->getErrorMessage().c_str(), e_udata->getErrorMessage().length()); */
			e_udata->closeAllFd();
			delete e_udata;
		}
	}
}

void EventLoop::e_pipeWriteCallback(struct kevent *e, Event *e_udata){}
void EventLoop::e_fileWriteCallback(struct kevent *e, Event *e_udata){}
