#include "HttpServer.hpp"

HttpServer& HttpServer::getInstance()
{
	static HttpServer instance;
	return instance;
}

void HttpServer::init() throw(std::runtime_error)
{
	this->H = static_cast<HttpBlock *>(Config::getInstance().getHTTPBlock());
	this->stringBuffer = new std::string();
	this->stringBuffer->resize(1024);

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


char* HttpServer::getHttpBuffer() { return this->HttpBuffer; }
std::string &HttpServer::getStringBuffer() { return *(this->stringBuffer); }

HttpServer::HttpServer() {}
HttpServer::~HttpServer() {}
HttpServer::HttpServer(const HttpServer &){}
HttpServer& HttpServer::operator=(const HttpServer &){ return *this; }
