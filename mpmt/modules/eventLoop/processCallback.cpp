#include "EventLoop.hpp"

void EventLoop::e_processCallback(struct kevent *e)
{
	std::cout<<"e_processCallback"<<std::endl;
	Event *e_udata = static_cast<Event *>(e->udata);

	if (e->fflags & NOTE_EXIT)
	{
		std::cerr<<"cgi process exit"<<std::endl;
		registerTmpFileReadEvent(e_udata);
	}
}
