#ifndef EVENTMANAGER_HPP
# define EVENTMANAGER_HPP

#include "Event.hpp"
#include <map>

class EventManager
{
private:
	EventManager();
	~EventManager();

	/* FD를 관리하는 rb tree.*/
	std::map<int, Event *> eventMap;

public:
	static EventManager& getInstance();


	void addEvent(Event *e);


	void deleteEvent(int fd);
	/**
	 * @brief check if client socket exists in event map
	 *
	 * @param fd
	 *
	 * @return 
	 */
	bool isClientSocketExists(int fd);

	Event *getEvent(int fd);
};


#endif
