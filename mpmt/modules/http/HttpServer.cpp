#include "HttpServer.hpp"

HttpServer& HttpServer::getInstance()
{
	static HttpServer instance;
	return instance;
}

void HttpServer::init() throw(std::runtime_error)
{
	this->H = static_cast<HttpBlock *>(Config::getInstance().getHTTPBlock());
	this->stringBuffer.resize(1024);
	this->locationDatasByPortMap = &(H->getLocationDatasByPort());

	std::map<int, std::vector<HttpLocationData *> *>& m = H->getLocationDatasByPort();

	/* port별로 server data를 순회한다.*/
	for (HttpBlock::locationDatasByPortMapIter it = m.begin(); it != m.end(); it++)
	{
		/* port별로 server socket event를 생성.*/
		int port = (*it).first;
		Event *e = Event::createNewServerSocketEvent(H->findLocationDatasByPort(port));

		int fd = e->getServerFd();
		this->serverSocketFd.push_back((fd));
		struct kevent kev;
	
		/* todo =>event 넣기*/
		EV_SET(&kev, fd, EVFILT_READ, EV_ADD, 0, 0, e);
		EventManager::getInstance().addEvent(e);
		this->kevents.push_back(kev);
	}
}

bool HttpServer::isServerSocket(int socket_fd)
{
	for (std::vector<int>::iterator it = this->serverSocketFd.begin(); it != this->serverSocketFd.end(); it++)
	{
		if (*it == socket_fd)
			return true;
	}
	return false;
}

HttpBlock::locationDatasByPortMap *HttpServer::getLocationDatasByPortMap() { return this->locationDatasByPortMap; }

std::vector<struct kevent> & HttpServer::getKevents() { return this->kevents; }


char* HttpServer::getHttpBuffer() { return this->HttpBuffer; }
std::string &HttpServer::getStringBuffer() { return this->stringBuffer; }

HttpServer::HttpServer() {}
HttpServer::~HttpServer() {}
HttpServer::HttpServer(const HttpServer &){}
HttpServer& HttpServer::operator=(const HttpServer &){ return *this; }
