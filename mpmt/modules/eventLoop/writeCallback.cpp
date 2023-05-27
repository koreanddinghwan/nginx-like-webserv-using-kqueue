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
		if (e_udata->statusCode >= 400)
		{
			write(e_udata->getClientFd(), e_udata->errorMessage.c_str(), e_udata->errorMessage.length());
			e_udata->closeAllFd();
			delete e_udata;
		}
	}
}
