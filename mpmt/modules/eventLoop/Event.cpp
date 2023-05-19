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

Event::~Event()
{}

Event::Event()
{}

Event::Event(Event &e)
{}

const Event& Event::operator=(const Event &e)
{return *this;}
