#ifndef EVENTLOOP_HPP
# define EVENTLOOP_HPP

#include <sys/event.h>
#include <stdexcept>
#include "../config/Config.hpp"
#include "../../interface/IBlock.hpp"

/**
 * @brief singleton eventloop
 */
class EventLoop
{
public:
	static EventLoop& getInstance();

	void initEventLoop();


private:
	int kq_fd;
	IBlock **blocks;


private:
	EventLoop();
	~EventLoop();
};


#endif
