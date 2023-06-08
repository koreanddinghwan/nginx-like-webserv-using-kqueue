#include "Event.hpp"
#include "EventLoop.hpp"
#include <cstdio>
#include <sys/event.h>
#include <sys/fcntl.h>
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
	EV_SET(&(dummyEvent), e->CtoPPipe[0], EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, e);
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
	std::cout<<"EVENTLOOP: registerClientSocketWriteEvent"<<std::endl;
	std::cout<<"internal_status: "<<e->internal_status<<std::endl;
	std::cout<<"status_code: "<<e->getStatusCode()<<std::endl;

	if ((e->internal_status != -1) && 
			(e->internal_status != e->getStatusCode()))
		e->setStatusCode(e->internal_status);

	e->setEventType(E_CLIENT_SOCKET);

	if (!static_cast<HttpreqHandler *>(e->getRequestHandler())->getHasSid())
	{
		HttpServer::getInstance().issueSessionId();
	}

	/**
	 * 최종적으로 client socket에 write하기전에 한 번만 호출되는 곳이므로, 여기서 response message와 wrotebyte를 설정해야함.
	 * */
	/**
	 * make response message here
	 * */
	std::cout<<"make response message here"<<std::endl;
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

void EventLoop::registerPipeWriteEvent(Event *e)
{
	std::cout<<"EVENTLOOP: registerPipeWriteEvent"<<std::endl;
	e->fileWroteByte = 0;
	e->setEventType(E_PIPE);
	EV_SET(&(dummyEvent), e->PtoCPipe[1], EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, e);
	if (kevent(this->kq_fd, &(dummyEvent), 1, NULL, 0, NULL) == -1) 
		throw std::runtime_error("Failed to register pipe write with kqueue\n");
}

void EventLoop::registerFileWriteEvent(Event *e)
{
	std::cout<<"EVENTLOOP: registerFileWriteEvent"<<std::endl;
	e->fileWroteByte = 0;
	e->setEventType(E_FILE);
	EV_SET(&(dummyEvent), e->file_fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, e);
	if (kevent(this->kq_fd, &(dummyEvent), 1, NULL, 0, NULL) == -1) 
		throw std::runtime_error("Failed to register file write with kqueue\n");
}

void EventLoop::registerTmpFileWriteEvent(Event *e)
{
	std::cout<<"EVENTLOOP: registerTmpFileWriteEvent"<<std::endl;
	e->fileWroteByte = 0;
	e->setEventType(E_TMP);
	EV_SET(&(dummyEvent), e->tmpOutFile, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, e);
	if (kevent(this->kq_fd, &(dummyEvent), 1, NULL, 0, NULL) == -1) 
	{
		std::cout<<"errno"<<errno<<std::endl;
		throw std::runtime_error("Failed to register file write with kqueue\n");
	}
}

void EventLoop::registerTmpFileReadEvent(Event *e)
{
	std::cout<<"EVENTLOOP: registerTmpFileReadEvent"<<std::endl;
	e->fileReadByte = 0;
	e->setEventType(E_TMP);
	stat(e->tmpInFileName.c_str(), &e->statBuf);
	EV_SET(&(dummyEvent), e->tmpInFile, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, e);
	if (kevent(this->kq_fd, &(dummyEvent), 1, NULL, 0, NULL) == -1) 
		throw std::runtime_error("Failed to register file write with kqueue\n");
}

/**
 * unregister event
 * */
void EventLoop::unregisterClientSocketReadEvent(Event *e)
{
	std::cout<<"unregister client socket read event"<<std::endl;
	EV_SET(&(dummyEvent), e->getClientFd(), EVFILT_READ, EV_DELETE | EV_DISABLE | EV_CLEAR, 0, 0, e);
	if (kevent(this->kq_fd, &(dummyEvent), 1, NULL, 0, NULL) == -1) 
		throw std::runtime_error("Failed to unregister client socket read with kqueue\n");
}

void EventLoop::unregisterPipeReadEvent(Event *e)
{
	std::cout<<"unregister pipe read event"<<std::endl;
	EV_SET(&(dummyEvent), e->CtoPPipe[0], EVFILT_READ, EV_DELETE | EV_DISABLE, 0, 0, e);
	kevent(this->kq_fd, &(dummyEvent), 1, NULL, 0, NULL); 
	close(e->CtoPPipe[0]);
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
	EV_SET(&(dummyEvent), e->getClientFd(), EVFILT_WRITE, EV_DELETE | EV_DISABLE , 0, 0, e);
	if (kevent(this->kq_fd, &(dummyEvent), 1, NULL, 0, NULL) == -1) 
		throw std::runtime_error("Failed to unregister client socket write with kqueue\n");

	delete (e->getResponseHandler());
	delete (e->getRequestHandler());

	e->setRequestHandler(new HttpreqHandler(e));
	e->setResponseHandler(new responseHandler(e));
	e->setStatusCode(200);
	e->internal_status = -1;
	e->internal_uri = "";
	e->locationData = NULL;
	registerClientSocketReadEvent(e);
}

void EventLoop::unregisterPipeWriteEvent(Event *e)
{
	std::cout<<"unregister pipe write event"<<std::endl;
	EV_SET(&(dummyEvent), e->PtoCPipe[1], EVFILT_WRITE, EV_DELETE | EV_DISABLE, 0, 0, e);
	(kevent(this->kq_fd, &(dummyEvent), 1, NULL, 0, NULL)); 
	close(e->PtoCPipe[1]);
}

void EventLoop::unregisterFileWriteEvent(Event *e)
{
	std::cout<<"unregister file write event"<<std::endl;
	EV_SET(&(dummyEvent), e->file_fd, EVFILT_WRITE, EV_DELETE | EV_DISABLE, 0, 0, e);
	if (kevent(this->kq_fd, &(dummyEvent), 1, NULL, 0, NULL) == -1) 
		throw std::runtime_error("Failed to unregister file write with kqueue\n");
	close(e->file_fd);
}


void EventLoop::unregisterTmpFileReadEvent(Event *e)
{
	std::cout<<"unregister file read event"<<std::endl;
	EV_SET(&(dummyEvent), e->tmpInFile, EVFILT_READ, EV_DELETE | EV_DISABLE, 0, 0, e);
	if (kevent(this->kq_fd, &(dummyEvent), 1, NULL, 0, NULL) == -1) 
		throw std::runtime_error("Failed to unregister file read with kqueue\n");
	close(e->tmpInFile);
	unlink(e->tmpInFileName.c_str());
	unlink(e->tmpOutFileName.c_str());
}


void EventLoop::unregisterTmpFileWriteEvent(Event *e)
{
	std::cout<<"unregister file write event"<<std::endl;
	EV_SET(&(dummyEvent), e->tmpOutFile, EVFILT_WRITE, EV_DELETE | EV_DISABLE, 0, 0, e);
	if (kevent(this->kq_fd, &(dummyEvent), 1, NULL, 0, NULL) == -1) 
		throw std::runtime_error("Failed to unregister file write with kqueue\n");
	close(e->tmpOutFile);
}
