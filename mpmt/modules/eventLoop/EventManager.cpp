#include "EventManager.hpp"
#include "Event.hpp"

EventManager::EventManager()
{}

EventManager::~EventManager()
{}

EventManager& EventManager::getInstance()
{
	static EventManager instance;
	return instance;
}

void EventManager::addEvent(Event *e)
{
	this->eventMap.insert(std::pair<int, Event *>(e->getServerFd(), e));
}

void EventManager::deleteEvent(int fd)
{
	std::map<int, Event *>::iterator it = this->eventMap.find(fd);
	if (it != this->eventMap.end())
	{
		delete it->second;
		this->eventMap.erase(fd);
	}
}

bool EventManager::isClientSocketExists(int fd)
{
	if (this->eventMap.find(fd) == this->eventMap.end())
		return false;
	return true;
}

Event *EventManager::getEvent(int fd)
{
	return this->eventMap[fd];
}
