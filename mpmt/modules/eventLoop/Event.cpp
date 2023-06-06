#include "Event.hpp"
#include "EventLoop.hpp"

Event::Event(t_ServerType t): logger("./logs/Eventlog.txt", std::ios::in | std::ios::out)
{
	this->cgiEnv.resize(20);
	for (int i =0; i < 20; i++)
		this->cgiEnv[i] = new char[100];
	this->statusCode = 200;
	this->server_socket_fd = -1;
	this->client_socket_fd = -1;
	this->r_serverEvent = false;
	this->r_clientEvent = false;
	this->r_fileEvent = false;
	this->r_pipeEvent = false;
	this->w_clientEvent = false;
	this->w_fileEvent = false;
	this->w_pipeEvent = false;
	this->PtoCPipe[0] = -1;
	this->PtoCPipe[1] = -1;
	this->CtoPPipe[0] = -1;
	this->CtoPPipe[1] = -1;
	this->file_fd = -1;
	this->serverType = t;
	this->statusCode = -1;
	this->internal_status = -1;
	this->locationData = NULL;
	logger<< "Event::Event(t_ServerType t)" << std::endl;
}

void Event::setServerType(t_ServerType t)
{memcpy(&this->serverType, &t, sizeof(t_ServerType));}

void Event::setSocketInfo(t_SocketInfo t)
{memcpy(&this->socketInfo, &t, sizeof(t_SocketInfo));}

void Event::setServerFd(int t)
{this->server_socket_fd  = t;}

void Event::setClientFd(int t)
{this->client_socket_fd = t;}

void Event::setEventType(t_EventType t)
{
	if (t == E_R_SERVER_SOCKET)
		this->r_serverEvent = true;
	else if (t == E_R_CLIENT_SOCKET)
		this->r_clientEvent = true;
	else if (t == E_R_FILE)
		this->r_fileEvent = true;
	else if (t == E_R_PIPE)
		this->r_pipeEvent = true;
	else if (t == E_W_CLIENT_SOCKET)
		this->w_clientEvent = true;
	else if (t == E_W_FILE)
		this->w_fileEvent = true;
	else if (t == E_W_PIPE)
		this->w_pipeEvent = true;
}

void Event::setRequestHandler(IHandler *t)
{this->requestHandler = t;}

void Event::setResponseHandler(IHandler *t)
{this->responseHandler = t;}

void Event::setServerDataByPort(int port)
{
	std::vector<HttpServerBlock *> *serverBlock = &(Config::getInstance().getHTTPBlock()->getHttpData().getServerBlock());

	for (int i = 0; i < serverBlock->size();i++)
	{
		if (serverBlock->at(i)->getServerData().getListen() == port)
		{
			std::cout<<port<<std::endl;
			this->serverData.push_back(&(serverBlock->at(i)->getServerData()));
			for (int j = 0; j < this->serverData.back()->getHttpLocationBlock().size(); j++)
			{
				this->serverData.back()->getLocationDatas().push_back( \
						&(this->serverData.back()->getHttpLocationBlock().at(j)->getLocationData())
						);
			}
		}
	}
	this->defaultServerData = this->serverData.at(0);
}

void Event::setServerData(std::vector<HttpServerData *> *t)
{
	this->serverData = *t;
	this->defaultServerData = t->at(0);
}

void Event::setDefaultServerData(HttpServerData *t)
{
	this->defaultServerData = t;
}

t_ServerType& Event::getServerType()
{return this->serverType;}

t_SocketInfo& Event::getSocketInfo()
{return this->socketInfo;}

int& Event::getServerFd()
{return this->server_socket_fd;}

int& Event::getClientFd()
{return this->client_socket_fd;}

t_EventType& Event::getEventType()
{return this->eventInfo;}

IHandler *Event::getRequestHandler()
{return this->requestHandler;}

IHandler *Event::getResponseHandler()
{return this->responseHandler;}

std::vector<HttpServerData *> *Event::getServerData()
{
	return &(this->serverData);
}

HttpServerData *Event::getDefaultServerData(){return this->defaultServerData;}

Event::~Event()
{
	//interface의 소멸자 호출하면, 연결된 소멸자 모두 호출.
	delete this->requestHandler;
	delete this->responseHandler;
	for (int i = 0; i < 20; i++)
		delete this->cgiEnv[i];
}

Event *Event::createNewClientSocketEvent(Event *e)
{

	Event *new_udata = new Event(HTTP_SERVER);

	/**
	 * client 는 serverSocket을 통해 연결되므로, serverSocketEvent가 가진 serverData를 그대로 가져온다.
	 * */
	new_udata->setServerData(e->getServerData());

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
	new_udata->setEventType(E_R_CLIENT_SOCKET);

	/**
	 * client socket의 fd를 등록
	 * */
	new_udata->setClientFd(client_fd);

	/**
	 * client socket의 socketInfo를 등록
	 * */
	new_udata->setSocketInfo(socketInfo);
	return new_udata;
}

Event *Event::createNewServerSocketEvent(int port)
{
	Event *e = new Event(HTTP_SERVER);
	e->setServerDataByPort(port);
	int fd;
	t_EventType event_type = E_R_SERVER_SOCKET;
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
	socketInfo.nagle_off = e->getServerData()->at(0)->getTcpNoDelay();
	if (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &socketInfo.nagle_off, sizeof(socketInfo.nagle_off)) == -1) 
		throw(std::runtime_error("setsockopt error"));

	/* set server_addr */
	socketInfo.socket_addr.sin_family = AF_INET;
	socketInfo.socket_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	socketInfo.socket_addr.sin_port = htons(port);

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
	return e;
}

void Event::closeAllFd()
{
	std::cout<<"closeAllFd\n";


	if (this->server_socket_fd != -1)
	{
		std::cout<<"|||close server_socket_fd\n";
		close(this->server_socket_fd);
	}
	if (this->client_socket_fd != -1)
	{
		std::cout<<"|||close client_socket_fd\n";
		close(this->client_socket_fd);
	}
	if (this->file_fd != -1)
	{
		std::cout<<"|||close file_fd\n";
		close(this->file_fd);
	}
}

int &Event::getStatusCode()
{return this->statusCode;}
void Event::setStatusCode(int t)
{this->statusCode = t;}

std::string *Event::getBuffer()
{return this->buffer;}
void Event::setBuffer(std::string *t)
{this->buffer = t;}

std::string &Event::getRoute()
{return this->route;}

void Event::setRoute(std::string t)
{this->route = t;}

/**
 * @brief client socket write전에 status code보고 errorpage세팅하는 함수.
 *
 * internal_uri에 errorpage를 넣고,
 * internal_method에 GET을 넣는다.
 * 이 internal_uri, method는 내부 location blcok 찾는 처리에서만 사용된다.
 * @return true if setted or false if not setted 
 */
bool Event::setErrorPage()
{
	if (!locationData)
		return false;

	std::map<int, std::string> *errorPage = this->locationData->getErrorPage();

	if (errorPage->size() == 0)
		return false;
	if (this->statusCode >= 400)
	{
		if (errorPage->find(this->statusCode) != errorPage->end())
		{
			this->internal_uri = errorPage->find(this->statusCode)->second;
			this->internal_method = "GET";
			return true;
		}
	}
	return false;
}

std::vector<char *> &Event::getCgiEnv()
{
	return this->cgiEnv;
}

/**
 * @deprecated
 * */
Event::Event(Event &e)
{}

/**
 * @DO NOT USE
 * */
const Event& Event::operator=(const Event &e)
{return *this;}

Event::Event()
{}
