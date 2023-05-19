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

void Event::setFdInfo(t_FdInfo t)
{memcpy(&this->fdInfo, &t, sizeof(t_FdInfo));}

t_ServerType& Event::getServerType()
{return this->serverType;}


t_SocketInfo& Event::getSocketInfo()
{return this->socketInfo;}

t_FdInfo& Event::getFdInfo()
{return this->fdInfo;}

Event::~Event()
{}

Event::Event()
{}

Event::Event(Event &e)
{}

const Event& Event::operator=(const Event &e)
{return *this;}
