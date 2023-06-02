#include "Event.hpp"
#include "EventLoop.hpp"
#include <sys/event.h>
#include <sys/socket.h>

void EventLoop::registerClientSocketReadEvent(Event *e)
{
	std::cout<<"EVENTLOOP: registerClientSocketReadEvent"<<std::endl;
	e->setEventType(E_CLIENT_SOCKET);
	//client socket을 읽기전용으로  kqueue에 등록
	EV_SET(&(dummyEvent), e->getClientFd(), EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, e);
	if (kevent(this->kq_fd, &(dummyEvent), 1, NULL, 0, NULL) == -1) 
		throw std::runtime_error("Failed to register client socket read with kqueue\n");
}

void EventLoop::registerPipeReadEvent(Event *e)
{
	std::cout<<"EVENTLOOP: registerPipeReadEvent"<<std::endl;
	e->setEventType(E_PIPE);
	EV_SET(&(dummyEvent), e->getPipeFd()[0], EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, e);
	if (kevent(this->kq_fd, &(dummyEvent), 1, NULL, 0, NULL) == -1) 
		throw std::runtime_error("Failed to register pipe read with kqueue\n");
}

void EventLoop::registerFileReadEvent(Event *e)
{
	std::cout<<"EVENTLOOP: registerFileReadEvent"<<std::endl;
	e->setEventType(E_FILE);
	e->fileReadByte = 0;
	EV_SET(&(dummyEvent), e->file_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, e);
	if (kevent(this->kq_fd, &(dummyEvent), 1, NULL, 0, NULL) == -1) 
		throw std::runtime_error("Failed to register file read with kqueue\n");
}

void EventLoop::registerClientSocketWriteEvent(Event *e)
{
	/**
	 * internal redirection
	 * */
	std::cout<<"EVENTLOOP: registerClientSocketWriteEvent"<<std::endl;
	std::cout<<"checking status code whether internal redirection needed"<<std::endl;
	if (e->getStatusCode() >= 400 && e->setErrorPage())
	{
		std::cout<<"internal redirection needed"<<std::endl;
		e->internal_status = e->getStatusCode();
		std::cout<<"internal status code: "<<e->internal_status<<std::endl;
		/**
		 * do internal redirection
		 * */
		e->getResource().clear();
		e->getRoute().clear();
		setHttpResponse(e);
		return ;
	} 
	// 404                            200
	if (e->internal_status >= 400) 
		e->setStatusCode(e->internal_status);
		/* e->setStatusCode(e->getStatusCode() == 200 ? 200 : e->internal_status); */
	
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

	std::cout<<"EVENTLOOP: registerClientSocketWriteEvent"<<std::endl;
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
	std::cout<<"EVENTLOOP: registerFileWriteEvent"<<std::endl;
	e->fileWroteByte = 0;
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
	std::cout<<"unregister pipe read event"<<std::endl;
	EV_SET(&(dummyEvent), e->getPipeFd()[0], EVFILT_READ, EV_DELETE | EV_DISABLE, 0, 0, e);
	if (kevent(this->kq_fd, &(dummyEvent), 1, NULL, 0, NULL) == -1) 
		throw std::runtime_error("Failed to unregister pipe read with kqueue\n");
	close(e->getPipeFd()[0]);
}

void EventLoop::unregisterFileReadEvent(Event *e)
{
	std::cout<<"unregister file read event"<<std::endl;
	EV_SET(&(dummyEvent), e->file_fd, EVFILT_READ, EV_DELETE | EV_DISABLE, 0, 0, e);
	if (kevent(this->kq_fd, &(dummyEvent), 1, NULL, 0, NULL) == -1) 
		throw std::runtime_error("Failed to unregister file read with kqueue\n");
	close(e->file_fd);
}

void EventLoop::unregisterClientSocketWriteEvent(Event *e)
{
	std::cout<<"unregister client socket write event"<<std::endl;
	EV_SET(&(dummyEvent), e->getClientFd(), EVFILT_WRITE, EV_DELETE | EV_DISABLE | EV_CLEAR, 0, 0, e);
	if (kevent(this->kq_fd, &(dummyEvent), 1, NULL, 0, NULL) == -1) 
		throw std::runtime_error("Failed to unregister client socket write with kqueue\n");

	delete e->getResponseHandler();
	delete e->getRequestHandler();

	e->setRequestHandler(new HttpreqHandler());
	e->setResponseHandler(new responseHandler(-1));
	e->setStatusCode(200);
	e->internal_status = 0;
	registerClientSocketReadEvent(e);
}

void EventLoop::unregisterPipeWriteEvent(Event *e)
{
	std::cout<<"unregister pipe write event"<<std::endl;
	EV_SET(&(dummyEvent), e->getPipeFd()[1], EVFILT_WRITE, EV_DELETE | EV_DISABLE, 0, 0, e);
	if (kevent(this->kq_fd, &(dummyEvent), 1, NULL, 0, NULL) == -1) 
		throw std::runtime_error("Failed to unregister pipe write with kqueue\n");
}

void EventLoop::unregisterFileWriteEvent(Event *e)
{
	std::cout<<"unregister file write event"<<std::endl;
	EV_SET(&(dummyEvent), e->file_fd, EVFILT_WRITE, EV_DELETE | EV_DISABLE, 0, 0, e);
	if (kevent(this->kq_fd, &(dummyEvent), 1, NULL, 0, NULL) == -1) 
		throw std::runtime_error("Failed to unregister file write with kqueue\n");
	close(e->file_fd);
}
