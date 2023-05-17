#include "./EventLoop.hpp"
#include <sys/_types/_uintptr_t.h>

EventLoop& EventLoop::getInstance() {
	static EventLoop instance;
	return instance;
}

void EventLoop::initEventLoop()
{
	this->kq_fd = kqueue();
	if (kq_fd == -1) 
		throw std::runtime_error("Failed to create kqueue\n");


	std::vector<HttpServer::portMapPair> &m = HttpServer::getInstance().getPortMap();

	// register event to kqueue
	for (int i = 0; i < HttpServer::getInstance().getKevents().size(); i++)
		registerSocketEvent(&(HttpServer::getInstance().getKevents()[i]));


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
			std::cout<<events[i].ident<<std::endl;
			if (HttpServer::getInstance().isServerSocket(fd))
			{
				std::cout<<"First Connection"<<std::endl;
				std::cout<<reinterpret_cast<uintptr_t>(events[i].udata)<<std::endl;
				std::vector<HttpLocationBlock *>* locData = static_cast<std::vector<HttpLocationBlock *>*>(events[i].udata);

				for (int i =0; i < locData->size(); i++)
				{
					(*locData)[i]->getLocationData().printLocationData();
				}
			}

		}
		

	}



}


void EventLoop::registerSocketEvent(struct kevent *kev)
{
	std::cout<<kev->ident<<std::endl;
	if (kevent(this->kq_fd, kev, 1, NULL, 0, NULL) == -1) 
		throw std::runtime_error("Failed to register socket event\n");
}

EventLoop::EventLoop() {}

EventLoop::~EventLoop() {}

void EventLoop::printCurrentData()
{
	std::vector<HttpServer::portMapPair> &m = HttpServer::getInstance().getPortMap();

	std::cout<<std::endl;
	std::cout<<std::endl;
	for (int i = 0; i < m.size(); i++)
	{
		std::cout<<"\033[33m";
		std::cout<<"============"<<i<<"============="<<std::endl;
		std::cout<<"========================="<<std::endl;

		std::cout<<"socket_fd: "<<m[i].first.first<<std::endl;
		std::cout<<"port number: "<<m[i].first.second<<std::endl;

		std::vector<HttpLocationBlock *> *v = m[i].second;

		std::cout<<"\033[35m";
		for (int j = 0; j < v->size(); j++)
		{
			std::cout<<"============["<<j<<"]============="<<std::endl;
			(*v)[j]->getLocationData().printLocationData();
			std::cout<<"=================================="<<std::endl;
		}
		std::cout<<"========================="<<std::endl;
		std::cout<<"========================="<<std::endl;
	}
}
