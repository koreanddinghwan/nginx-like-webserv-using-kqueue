#ifndef EVENTLOOP_HPP
# define EVENTLOOP_HPP

#include <sys/event.h>
#include <stdexcept>
#include "../config/Config.hpp"
#include "../../interface/IBlock.hpp"
#include "../http/HttpServer.hpp"

/**
 * @brief singleton eventloop
 */
class EventLoop
{
public:
	/**
	 * @brief get instance of eventloop
	 *
	 * @return eventloop instance
	 */
	static EventLoop& getInstance();

	/**
	 * @brief init eventloop
	 * 핵심모듈
	 */
	void initEventLoop();

private:
	int kq_fd;

private:
	EventLoop();
	~EventLoop();
	void printCurrentData();
	void registerSocketEvent(struct kevent *kev);
};


#endif
