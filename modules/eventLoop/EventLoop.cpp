#include "./EventLoop.hpp"
#include "Event.hpp"
#include <cstdio>
#include <exception>
#include <stdexcept>
#include <sys/_types/_ssize_t.h>
#include <sys/_types/_uintptr_t.h>
#include <sys/event.h>

void printEvent(void *e);


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

	// int max_events = static_cast<EventBlock *>(Config::getInstance().getEventBlock())->getEventConfigData().worker_connections; 

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
			printEvent(events[i].udata);
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


void printEvent(void *a)
{
	Event *e = static_cast<Event *>(a);
	HttpreqHandler *reqHandler = static_cast<HttpreqHandler *>(e->getRequestHandler());
	std::cout<<"\033[33m"<<"===================="<<std::endl;
	std::cout<<"Event Information"<<std::endl;
	std::cout<<"===================="<<std::endl;
	if (e->getServerType() == HTTP_SERVER)
		std::cout<<"Server Type : HTTP_SERVER"<<std::endl;
	if (e->getEventType() == E_SERVER_SOCKET)
		std::cout<<"Event Type : E_SERVER_SOCKET"<<std::endl;
	else if (e->getEventType() == E_CLIENT_SOCKET)
		std::cout<<"Event Type : E_CLIENT_SOCKET"<<std::endl;
	else if (e->getEventType() == E_FILE)
		std::cout<<"Event Type : E_FILE"<<std::endl;
	else if (e->getEventType() == E_PIPE)
		std::cout<<"Event Type : E_PIPE"<<std::endl;
	if (e->getEventType() != E_SERVER_SOCKET)
	{
		std::cout<<"Server Socket fd : "<<e->getServerFd()<<std::endl;
		std::cout<<"Client Socket fd : "<<e->getClientFd()<<std::endl;
		std::cout<<"File fd : "<<e->file_fd<<std::endl;
		std::cout<<"Status Code : "<<e->getStatusCode()<<std::endl;
		std::cout<<"Route : "<<e->getRoute()<<std::endl;
		std::cout<<"Uri : "<<reqHandler->getRequestInfo().path<<std::endl;
		std::cout<<"Method : "<<reqHandler->getRequestInfo().method<<std::endl;
		std::cout<<"===================="<<std::endl;
	}
}


EventLoop::EventLoop() {}

EventLoop::~EventLoop() {}
