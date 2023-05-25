#ifndef EVENTMANAGER_HPP
# define EVENTMANAGER_HPP

#include "Event.hpp"
#include <map>

/**
 * @brief EventManager는 Event를 관리하는 클래스이다.
 * EventManager는 싱글톤으로 구현되어 있다.
 * EventManager는 Event를 추가하거나 삭제할 수 있다.
 * EventManager는 Event를 fd를 통해 찾을 수 있다.
 * EventManager는 Event를 fd를 통해 삭제할 수 있다.
 * EventManager는 Event를 fd를 통해 존재하는지 확인할 수 있다.
 */
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
