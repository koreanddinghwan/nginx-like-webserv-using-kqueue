#include "./EventLoop.hpp"
#include "Event.hpp"
#include <exception>
#include <stdexcept>
#include <sys/_types/_ssize_t.h>
#include <sys/_types/_uintptr_t.h>
#include <sys/event.h>

EventLoop& EventLoop::getInstance() {
	static EventLoop instance;
	return instance;
}

void EventLoop::initEventLoop()
{
	this->kq_fd = kqueue();
	if (kq_fd == -1) 
		throw std::runtime_error("Failed to create kqueue\n");

	// initially register event to kqueue
	for (int i = 0; i < HttpServer::getInstance().getKevents().size(); i++)
		registerInitialEvent(&(HttpServer::getInstance().getKevents()[i]));

	int max_events = static_cast<EventBlock *>(Config::getInstance().getEventBlock())->getEventConfigData().worker_connections; 

	/*
	 * EventLoop
	 * */
	while (true)
	{
		struct kevent events[max_events];
		int nevents;
		if ((nevents = kevent(this->kq_fd, NULL, 0, events, max_events, NULL)) == -1) 
			throw std::runtime_error("Failed to kevent\n");

		for (int i = 0; i < nevents; i++)
		{
			int fd = events[i].ident;
			std::cout<<"Current Event Ident: ["<<events[i].ident<<"]"<<std::endl;
			try {
				if (events[i].filter & EVFILT_READ)
					readCallback(events + i);
				else if (events[i].filter & EVFILT_WRITE)
					writeCallback(events + i);
				else
				{
					std::cout<<"unknown event Filter"<<std::endl;
					std::cout<<events[i].filter<<std::endl;
					std::cout<<events[i].flags<<std::endl;
					std::cout<<"end"<<std::endl;
				}
			} catch (std::exception &e)
			{
				std::cerr << e.what() << std::endl;
				continue;
			}
		}
	}
}

void EventLoop::registerInitialEvent(struct kevent *kev)
{
	if (kevent(this->kq_fd, kev, 1, NULL, 0, NULL) == -1) 
		throw std::runtime_error("Failed to register socket event\n");
}


EventLoop::EventLoop() {}

EventLoop::~EventLoop() {}

/**
 * @brief depreciated
 *
 * @param e
 */
void EventLoop::disconnectionCallback(struct kevent *e)
{
	EV_SET(&(this->dummyEvent), e->ident, e->filter, EV_DELETE, 0, 0, NULL);
	if (kevent(this->kq_fd, &(this->dummyEvent), 1, NULL, 0, NULL) == -1) 
		throw std::runtime_error("Failed to delete with kqueue\n");
}
