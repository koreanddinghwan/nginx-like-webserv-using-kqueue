#include "Event.hpp"
#include "EventLoop.hpp"
#include <sys/event.h>
#include <sys/socket.h>

void EventLoop::registerClientSocketReadEvent(Event *e)
{
	e->setEventType(E_CLIENT_SOCKET);
	//client socket을 읽기전용으로  kqueue에 등록
	EV_SET(&(dummyEvent), e->getClientFd(), EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, e);
	if (kevent(this->kq_fd, &(dummyEvent), 1, NULL, 0, NULL) == -1) 
		throw std::runtime_error("Failed to register client socket read with kqueue\n");
}

void EventLoop::registerPipeReadEvent(Event *e)
{
	e->setEventType(E_PIPE);
	EV_SET(&(dummyEvent), e->getPipeFd()[0], EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, e);
	if (kevent(this->kq_fd, &(dummyEvent), 1, NULL, 0, NULL) == -1) 
		throw std::runtime_error("Failed to register pipe read with kqueue\n");
}

void EventLoop::registerFileReadEvent(Event *e)
{
	e->setEventType(E_FILE);
	e->fileReadByte = 0;
	EV_SET(&(dummyEvent), e->file_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, e);
	if (kevent(this->kq_fd, &(dummyEvent), 1, NULL, 0, NULL) == -1) 
		throw std::runtime_error("Failed to register file read with kqueue\n");
}

void EventLoop::registerClientSocketWriteEvent(Event *e)
{
	e->setEventType(E_CLIENT_SOCKET);
	/**
	 * 최종적으로 client socket에 write하기전에 한 번만 호출되는 곳이므로, 여기서 response message와 wrotebyte를 설정해야함.
	 * */
	/**
	 * make response message here
	 * */
	e->getResponseHandler()->handle(e);

	/**
	 * wrote byte set;
	 * */
	e->wrote = 0;

	//client socket을 쓰기전용으로  kqueue에 등록
	EV_SET(&(dummyEvent), e->getClientFd(), EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, e);
	if (kevent(this->kq_fd, &(dummyEvent), 1, NULL, 0, NULL) == -1) 
		throw std::runtime_error("Failed to register client socket write with kqueue\n");
}

/* void EventLoop::registerPipeWriteEvent(Event *e) */
/* { */
/* 	EV_SET(&(dummyEvent), e->getPipeFd()[1], EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, e); */
/* 	if (kevent(this->kq_fd, &(dummyEvent), 1, NULL, 0, NULL) == -1) */ 
/* 		throw std::runtime_error("Failed to register pipe write with kqueue\n"); */
/* } */

void EventLoop::registerFileWriteEvent(Event *e)
{
	e->setEventType(E_FILE);
	EV_SET(&(dummyEvent), e->file_fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, e);
	if (kevent(this->kq_fd, &(dummyEvent), 1, NULL, 0, NULL) == -1) 
		throw std::runtime_error("Failed to register file write with kqueue\n");
}


/**
 * unregister event
 * */
void EventLoop::unregisterClientSocketReadEvent(Event *e)
{
	std::cout<<"unregister client socket read event"<<std::endl;
	EV_SET(&(dummyEvent), e->getClientFd(), EVFILT_READ, EV_DELETE | EV_DISABLE, 0, 0, e);
	if (kevent(this->kq_fd, &(dummyEvent), 1, NULL, 0, NULL) == -1) 
		throw std::runtime_error("Failed to unregister client socket read with kqueue\n");
}

void EventLoop::unregisterPipeReadEvent(Event *e)
{
	EV_SET(&(dummyEvent), e->getPipeFd()[0], EVFILT_READ, EV_DELETE | EV_DISABLE, 0, 0, e);
	if (kevent(this->kq_fd, &(dummyEvent), 1, NULL, 0, NULL) == -1) 
		throw std::runtime_error("Failed to unregister pipe read with kqueue\n");
	close(e->getPipeFd()[0]);
}

void EventLoop::unregisterFileReadEvent(Event *e)
{
	EV_SET(&(dummyEvent), e->getFileFd(), EVFILT_READ, EV_DELETE | EV_DISABLE, 0, 0, e);
	if (kevent(this->kq_fd, &(dummyEvent), 1, NULL, 0, NULL) == -1) 
		throw std::runtime_error("Failed to unregister file read with kqueue\n");
}

void EventLoop::unregisterClientSocketWriteEvent(Event *e)
{
	std::cout<<"unregister client socket write event"<<std::endl;
	EV_SET(&(dummyEvent), e->getClientFd(), EVFILT_WRITE, EV_DELETE | EV_DISABLE | EV_CLEAR, 0, 0, e);
	if (kevent(this->kq_fd, &(dummyEvent), 1, NULL, 0, NULL) == -1) 
		throw std::runtime_error("Failed to unregister client socket write with kqueue\n");

	e->closeAllFd();
	//delete event
	delete e;
}

void EventLoop::unregisterPipeWriteEvent(Event *e)
{
	EV_SET(&(dummyEvent), e->getPipeFd()[1], EVFILT_WRITE, EV_DELETE | EV_DISABLE, 0, 0, e);
	if (kevent(this->kq_fd, &(dummyEvent), 1, NULL, 0, NULL) == -1) 
		throw std::runtime_error("Failed to unregister pipe write with kqueue\n");
}

void EventLoop::unregisterFileWriteEvent(Event *e)
{
	EV_SET(&(dummyEvent), e->getFileFd(), EVFILT_WRITE, EV_DELETE | EV_DISABLE, 0, 0, e);
	if (kevent(this->kq_fd, &(dummyEvent), 1, NULL, 0, NULL) == -1) 
		throw std::runtime_error("Failed to unregister file write with kqueue\n");
}
