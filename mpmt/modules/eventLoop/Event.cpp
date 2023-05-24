#include "Event.hpp"


Event::Event(t_ServerType t)
{
	/* @todo 
	 *
	 * set handler object by server type
	 * */
	this->serverType = t;

	switch (this->serverType) {
		case (t_ServerType::HTTP_SERVER):
			this->requestHandler = new HttpreqHandler();
			break;
		default:
			break;
	}
}

void Event::setServerType(t_ServerType t)
{memcpy(&this->serverType, &t, sizeof(t_ServerType));}

void Event::setSocketInfo(t_SocketInfo t)
{memcpy(&this->socketInfo, &t, sizeof(t_SocketInfo));}

void Event::setFd(int t)
{memcpy(&this->fd, &t, sizeof(int));}

void Event::setEventType(t_EventType t)
{this->eventInfo = t;}

void Event::setConfigData(std::vector<HttpLocationData *> *t)
{this->configData = t;}

t_ServerType& Event::getServerType()
{return this->serverType;}

t_SocketInfo& Event::getSocketInfo()
{return this->socketInfo;}

int& Event::getFd()
{return this->fd;}

t_EventType& Event::getEventType()
{return this->eventInfo;}

Event::~Event()
{}

Event::Event()
{}

Event::Event(Event &e)
{
	this->serverType = e.getServerType();
	this->socketInfo = e.getSocketInfo();
	this->fd = e.getFd();
	this->eventInfo = e.getEventType();
	this->requestHandler = e.getRequestHandler();
	this->configData = e.getConfigData();
}

const Event& Event::operator=(const Event &e)
{return *this;}
