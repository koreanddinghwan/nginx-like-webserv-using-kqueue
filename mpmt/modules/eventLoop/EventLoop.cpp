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

	this->initServerEvents();

	int max_events = static_cast<EventBlock *>(Config::getInstance().getEventBlock())->getEventConfigData().worker_connections; 

	/*
	 * EventLoop
	 * */
	while (true)
	{
		struct kevent events[8];
		int nevents;
		if ((nevents = kevent(this->kq_fd, NULL, 0, events, 8, NULL)) == -1) 
			throw std::runtime_error("Failed to kevent\n");

		for (int i = 0; i < nevents; i++)
		{
			int fd = events[i].ident;
			try {
				if (events[i].filter == EVFILT_READ)
					readCallback(events + i);
				else if (events[i].filter == EVFILT_WRITE)
					writeCallback(events + i);
			} catch (std::exception &e)
			{
				continue;
			}
		}
	}
}

void EventLoop::initServerEvents()
{
	// initially register event to kqueue
	for (std::vector<struct kevent>::iterator it = HttpServer::getInstance().getKevents().begin(); it != HttpServer::getInstance().getKevents().end(); it++)
	{
		if (kevent(this->kq_fd, &(*it), 1, NULL, 0, NULL) == -1) 
			throw std::runtime_error("Failed to register socket event\n");
	}
}


EventLoop::EventLoop() {}

EventLoop::~EventLoop() {}
