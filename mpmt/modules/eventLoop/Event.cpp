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
