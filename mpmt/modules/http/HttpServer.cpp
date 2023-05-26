#include "HttpServer.hpp"
#include <vector>

HttpServer& HttpServer::getInstance()
{
	static HttpServer instance;
	return instance;
}

void HttpServer::makeSocketByLocationData(std::vector<HttpLocationData *> *m, Event *e)
{
	int fd;
	t_EventType E_SERVER_SOCKET;
	t_SocketInfo socketInfo;

	/* create socket */
	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		throw(std::runtime_error("socket error"));

	/* set socket option */
	socketInfo.reUseAddr = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &(socketInfo.reUseAddr), sizeof(socketInfo.reUseAddr)) == -1)
		throw(std::runtime_error("setsockopt error"));


	socketInfo.keep_alive = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &socketInfo.keep_alive, sizeof(socketInfo.keep_alive)) == -1)
		throw(std::runtime_error("setsockopt error"));

	// Disable Nagle algorithm
	socketInfo.nagle_off = (*m)[0]->getTcpNoDelay();
	if (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &socketInfo.nagle_off, sizeof(socketInfo.nagle_off)) == -1) 
		throw(std::runtime_error("setsockopt error"));

	/* set server_addr */
	socketInfo.socket_addr.sin_family = AF_INET;
	socketInfo.socket_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	socketInfo.socket_addr.sin_port = htons((*m)[0]->getListen());

	/* bind */
	if (bind(fd, (struct sockaddr *)&socketInfo.socket_addr, sizeof((socketInfo.socket_addr))) == -1)
		throw(std::runtime_error("bind error"));

	/* set fd to non-blocking */
	int flags = fcntl(fd, F_GETFL, 0);
	fcntl(fd, F_SETFL, flags | O_NONBLOCK);

	/* listen */
	if (listen(fd, 1024) == -1)
		throw(std::runtime_error("listen error"));

	e->setServerFd(fd);
	e->setSocketInfo(socketInfo);
	e->setEventType(E_SERVER_SOCKET);
	e->setLocationData(m);
	e->setDefaultServerData((Config::getInstance().getHTTPBlock()->getDefaultServerData()));
}

void HttpServer::init() throw(std::runtime_error)
{
	/*
	 * @ToDo
	 * */
	/* this->requestHandler = new RequestHandler(); */
	/* this->requestHandler = new ResponseHandler(); */
	this->H = static_cast<HttpBlock *>(Config::getInstance().getHTTPBlock());
	this->locationDatasByPortMap = &(H->getLocationDatasByPort());

	std::map<int, std::vector<HttpLocationData *> *>& m = H->getLocationDatasByPort();

	/* port별로 server data를 순회한다.*/
	for (HttpBlock::locationDatasByPortMapIter it = m.begin(); it != m.end(); it++)
	{
		/* port별로 server socket event를 생성.*/
		int port = (*it).first;
		Event *e = new Event(HTTP_SERVER);
		makeSocketByLocationData(H->findLocationDatasByPort(port), e);
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

HttpServer::HttpServer() {}
HttpServer::~HttpServer() {}
HttpServer::HttpServer(const HttpServer &){}
HttpServer& HttpServer::operator=(const HttpServer &){ return *this; }
