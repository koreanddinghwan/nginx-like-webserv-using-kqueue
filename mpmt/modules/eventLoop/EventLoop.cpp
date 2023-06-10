#include "./EventLoop.hpp"
#include "Event.hpp"
#include <exception>
#include <stdexcept>
#include <sys/_types/_ssize_t.h>
#include <sys/_types/_uintptr_t.h>
#include <sys/event.h>


void printEvent(struct kevent *e)
{
	std::cout<<"===================="<<std::endl;
	std::cout<<"ident: "<<e->ident<<std::endl;
	std::cout<<"filter: "<<e->filter<<std::endl;
	std::cout<<"flag: "<<e->flags<<std::endl;
	std::cout<<"fflag: "<<e->fflags<<std::endl;
	std::cout<<"data: "<<e->data<<std::endl;
	std::cout<<"udata: "<<e->udata<<std::endl;
}

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
		struct kevent events[1];
		int nevents;
		(nevents = kevent(this->kq_fd, NULL, 0, events, 1, NULL));

		for (int i = 0; i < nevents; i++)
		{
			int fd = events[i].ident;
			printEvent(events + i);
			try {
				if (events[i].filter == EVFILT_READ)
					readCallback(events + i);
				else if (events[i].filter == EVFILT_WRITE)
					writeCallback(events + i);
				else if (events[i].filter == EVFILT_PROC)
					e_processCallback(events + i);
			} catch (std::exception &e)
			{
				std::cout<<e.what()<<std::endl;
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
		std::cout<<"register initial event"<<std::endl;
		std::cout<<it->ident<<std::endl;
		std::cout<<it->filter<<std::endl;
		std::cout<<it->flags<<std::endl;
		std::cout<<it->fflags<<std::endl;
		std::cout<<it->data<<std::endl;
		std::cout<<it->udata<<std::endl;
		if (kevent(this->kq_fd, &(*it), 1, NULL, 0, NULL) == -1) 
			throw std::runtime_error("Failed to register socket event\n");
	}
}


EventLoop::EventLoop() {}

EventLoop::~EventLoop() {}
