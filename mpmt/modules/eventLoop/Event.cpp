#include "Event.hpp"
#include "EventManager.hpp"


Event::Event(t_ServerType t)
{
	/* @todo 
	 *
	 * set handler object by server type
	 * */
	this->serverType = t;

	/**
	 * set default handler and default server data
	 * by server type!
	 * */
	switch (this->serverType) {
		case (t_ServerType::HTTP_SERVER):
			this->defaultServerData = 
				static_cast<HttpServerData *>(Config::getInstance().getHTTPBlock()->getConfigData());
			break;
		default:
			break;
	}
}

void Event::setServerType(t_ServerType t)
{memcpy(&this->serverType, &t, sizeof(t_ServerType));}

void Event::setSocketInfo(t_SocketInfo t)
{memcpy(&this->socketInfo, &t, sizeof(t_SocketInfo));}

void Event::setServerFd(int t)
{this->server_socket_fd  = t;}

void Event::setClientFd(int t)
{this->client_socket_fd = t;}

void Event::setPipeFd(int t)
{this->pipe_fd = t;}

void Event::setFileFd(int t)
{this->file_fd = t;}

void Event::setEventType(t_EventType t)
{this->eventInfo = t;}

void Event::setRequestHandler(IHandler *t)
{this->requestHandler = t;}

void Event::setResponseHandler(IHandler *t)
{this->responseHandler = t;}

void Event::setLocationData(std::vector<HttpLocationData *> *t)
{this->locationData = t;}



t_ServerType& Event::getServerType()
{return this->serverType;}

t_SocketInfo& Event::getSocketInfo()
{return this->socketInfo;}

int& Event::getServerFd()
{return this->server_socket_fd;}

int& Event::getClientFd()
{return this->client_socket_fd;}

int& Event::getPipeFd()
{return this->pipe_fd;}

int& Event::getFileFd()
{return this->file_fd;}

t_EventType& Event::getEventType()
{return this->eventInfo;}

IHandler *Event::getRequestHandler()
{return this->requestHandler;}

IHandler *Event::getResponseHandler()
{return this->responseHandler;}

std::vector<HttpLocationData *> *Event::getLocationData() {return this->locationData;}

HttpServerData *Event::getDefaultServerData(){return this->defaultServerData;}

Event::~Event()
{
	//interface의 소멸자 호출하면, 연결된 소멸자 모두 호출.
	delete this->requestHandler;
	delete this->responseHandler;
}

Event *Event::createNewClientSocketEvent(Event *e)
{

	Event *new_udata = new Event(HTTP_SERVER);

	EventManager::getInstance().addEvent(new_udata);
	t_SocketInfo socketInfo;
	int sockfd = e->getServerFd();
	socklen_t cliLen = sizeof(socketInfo.socket_addr);
	int client_fd;


	client_fd = accept(sockfd, (struct sockaddr *)&(socketInfo.socket_addr), &cliLen);
	if (client_fd == -1)
	{
		delete new_udata;
		throw std::runtime_error("Failed to accept client socket\n");
	}

	/**
	 * event에 전달할 udata 채우기!
	 * */
	/* client socket에 대한 read event입니다.*/
	new_udata->setEventType(E_CLIENT_SOCKET);

	/**
	 * client socket의 fd를 등록
	 * */
	new_udata->setClientFd(client_fd);

	/**
	 * client socket의 socketInfo를 등록
	 * */
	new_udata->setSocketInfo(socketInfo);

	/**
	 * client socket에 Location data 등록
	 * 현재 udata의 Event에 이미 port별로 location data가 있음.
	 * 그대로 복사
	 * default server data는 생성자에서 초기화됨.
	 * */
	new_udata->setLocationData(e->getLocationData());
	return new_udata;
}

Event *Event::createNewServerSocketEvent(t_locationData *m)
{
	Event *e = new Event(HTTP_SERVER);
	int fd;
	t_EventType event_type = E_SERVER_SOCKET;
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
	e->setEventType(event_type);
	e->setLocationData(m);
	return e;
}

/**
 * @deprecated
 * */
Event::Event(Event &e)
{
	/* this->serverType = e.getServerType(); */
	/* this->socketInfo = e.getSocketInfo(); */

	/* this->server_socket_fd = e.getServerFd(); */
	/* this->client_socket_fd = e.getClientFd(); */
	/* this->pipe_fd = e.getPipeFd(); */
	/* this->file_fd = e.getFileFd(); */

	/* this->eventInfo = e.getEventType(); */
	/* this->requestHandler = e.getRequestHandler(); */
	/* this->locationData = e.getLocationData(); */
	/* this->defaultServerData = e.getDefaultServerData(); */
}

/**
 * @DO NOT USE
 * */
const Event& Event::operator=(const Event &e)
{return *this;}

Event::Event()
{}
