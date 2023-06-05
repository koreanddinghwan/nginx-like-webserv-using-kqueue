#include "HttpServer.hpp"

unsigned int hash(unsigned int x) 
{
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = (x >> 16) ^ x;
    return x;
}

HttpServer& HttpServer::getInstance()
{
	static HttpServer instance;
	return instance;
}

void HttpServer::init() throw(std::runtime_error)
{
	this->H = static_cast<HttpBlock *>(Config::getInstance().getHTTPBlock());

	for (std::vector<int>::iterator it = this->H->getIdenticalPorts().begin(); it != this->H->getIdenticalPorts().end(); it++)
	{
		int port = (*it);
		Event *e = Event::createNewServerSocketEvent(port);

		struct kevent kev;
		EV_SET(&kev, e->getServerFd(), EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, e);
		this->kevents.push_back(kev);
	}
}

std::vector<struct kevent> & HttpServer::getKevents() { return this->kevents; }

unsigned int HttpServer::issueSessionId()
{
	return hash(static_cast<unsigned int>(time(NULL)));
}

char* HttpServer::getHttpBuffer() { return this->HttpBuffer; }

HttpServer::HttpServer() {}
HttpServer::~HttpServer() {}
HttpServer::HttpServer(const HttpServer &){}
HttpServer& HttpServer::operator=(const HttpServer &){ return *this; }
