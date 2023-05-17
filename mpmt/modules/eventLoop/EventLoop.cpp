#include "./EventLoop.hpp"

EventLoop& EventLoop::getInstance() {
	static EventLoop instance;
	return instance;
}

void EventLoop::initEventLoop()
{
}


EventLoop::EventLoop() {
	this->kq_fd = kqueue();
	if (kq_fd == -1) 
		throw std::runtime_error("Failed to create kqueue\n");
}

EventLoop::~EventLoop() {}
