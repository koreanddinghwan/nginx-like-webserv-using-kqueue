#include "Event.hpp"
#include "EventLoop.hpp"
#include <cstdio>
#include <sys/event.h>
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <unistd.h>

void EventLoop::registerClientSocketReadEvent(Event *e)
{
	e->setEventType(E_CLIENT_SOCKET);
	EV_SET(&(dummyEvent), e->getClientFd(), EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, e);
	(kevent(this->kq_fd, &(dummyEvent), 1, NULL, 0, NULL)); 
}

void EventLoop::registerPipeReadEvent(Event *e)
{
	e->setEventType(E_PIPE);
	e->fileReadByte = 0;
	EV_SET(&(dummyEvent), e->CtoPPipe[0], EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, e);
	(kevent(this->kq_fd, &(dummyEvent), 1, NULL, 0, NULL)); 
}

void EventLoop::registerFileReadEvent(Event *e)
{
	e->setEventType(E_FILE);
	e->fileReadByte = 0;
	EV_SET(&(dummyEvent), e->file_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, e);
	(kevent(this->kq_fd, &(dummyEvent), 1, NULL, 0, NULL)); 
}

void EventLoop::registerClientSocketWriteEvent(Event *e)
{
	if ((e->internal_status != -1) && 
			(e->internal_status != e->getStatusCode()))
		e->setStatusCode(e->internal_status);

	e->setEventType(E_CLIENT_SOCKET);

	if (!static_cast<HttpreqHandler *>(e->getRequestHandler())->getHasSid())
		HttpServer::getInstance().issueSessionId();


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
	EV_SET(&(dummyEvent), e->getClientFd(), EVFILT_WRITE, EV_ADD | EV_ENABLE | EV_CLEAR, 0, 0, e);
	(kevent(this->kq_fd, &(dummyEvent), 1, NULL, 0, NULL)); 
}

void EventLoop::registerPipeWriteEvent(Event *e)
{
	e->fileWroteByte = 0;
	e->setEventType(E_PIPE);
	EV_SET(&(dummyEvent), e->PtoCPipe[1], EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, e);
	(kevent(this->kq_fd, &(dummyEvent), 1, NULL, 0, NULL)); 
}

void EventLoop::registerFileWriteEvent(Event *e)
{
	e->fileWroteByte = 0;
	e->setEventType(E_FILE);
	EV_SET(&(dummyEvent), e->file_fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, e);
	(kevent(this->kq_fd, &(dummyEvent), 1, NULL, 0, NULL)); 
}

void EventLoop::registerTmpFileWriteEvent(Event *e)
{
	e->fileWroteByte = 0;
	e->setEventType(E_TMP);
	EV_SET(&(dummyEvent), e->tmpOutFile, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, e);
	(kevent(this->kq_fd, &(dummyEvent), 1, NULL, 0, NULL)); 
}

void EventLoop::registerTmpFileReadEvent(Event *e)
{
	printf("tmpfile read event register\n");
	printf("%d, %d, %d, %d", e->oldStdOut, e->oldStdin, STDIN_FILENO, STDOUT_FILENO);
	printf("%d, %d", e->tmpInFile, e->tmpOutFile);
	std::cerr<<"tmp file read event register"<<std::endl;
	if ((e->tmpInFile = open(e->tmpInFileName.c_str(), O_RDONLY)) == -1)
		std::cerr<<"error open"<<e->tmpInFileName<< errno<<std::endl;

	int flag;

	flag = fcntl(e->tmpInFile, F_GETFL, 0);
	flag |= O_NONBLOCK;
	if (fcntl(e->tmpInFile, F_SETFL, flag) == -1)
		std::cerr<<"error fcntl"<<e->tmpInFileName<< errno<<std::endl;
	e->fileReadByte = 0;
	e->setEventType(E_TMP);
	EV_SET(&(dummyEvent), e->tmpInFile, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, e);
	(kevent(this->kq_fd, &(dummyEvent), 1, NULL, 0, NULL));
}

void EventLoop::registerCgiExitEvent(Event *e)
{
	std::cout<<"register cgi exit event"<<std::endl;
	EV_SET(&(dummyEvent), e->childPid, EVFILT_PROC, EV_ADD | EV_ENABLE | EV_ONESHOT, NOTE_EXIT, 0, e);
	(kevent(this->kq_fd, &(dummyEvent), 1, NULL, 0, NULL));
		std::cerr<<"child proc exit event"<<errno<<std::endl;
}


/**
 * unregister event
 * */
void EventLoop::unregisterClientSocketReadEvent(Event *e)
{
	EV_SET(&(dummyEvent), e->getClientFd(), EVFILT_READ, EV_DELETE | EV_DISABLE | EV_CLEAR, 0, 0, e);
	(kevent(this->kq_fd, &(dummyEvent), 1, NULL, 0, NULL)); 
}

void EventLoop::unregisterPipeReadEvent(Event *e)
{
	EV_SET(&(dummyEvent), e->CtoPPipe[0], EVFILT_READ, EV_DELETE | EV_DISABLE, 0, 0, e);
	kevent(this->kq_fd, &(dummyEvent), 1, NULL, 0, NULL); 
	close(e->CtoPPipe[0]);
}

void EventLoop::unregisterFileReadEvent(Event *e)
{
	EV_SET(&(dummyEvent), e->file_fd, EVFILT_READ, EV_DELETE | EV_DISABLE | EV_CLEAR, 0, 0, e);
	(kevent(this->kq_fd, &(dummyEvent), 1, NULL, 0, NULL)); 
	close(e->file_fd);
}

void EventLoop::unregisterClientSocketWriteEvent(Event *e)
{
	printf( "%s\n", "unregisterClientSocketWriteEvent");
	EV_SET(&(dummyEvent), e->getClientFd(), EVFILT_WRITE, EV_DELETE | EV_DISABLE | EV_CLEAR , 0, 0, e);
	(kevent(this->kq_fd, &(dummyEvent), 1, NULL, 0, NULL)); 

	delete (e->getResponseHandler());
	delete (e->getRequestHandler());

	printf("reset event\n");
	printf("client fd : %d", e->getClientFd());
	e->setRequestHandler(new HttpreqHandler(e));
	e->setResponseHandler(new responseHandler(e));
	e->setStatusCode(200);
	e->internal_status = -1;
	e->internal_uri = "";
	e->internal_method = "";
	e->locationData = NULL;
	e->tmpOutFile = -1;
	e->tmpInFile = -1;
	e->childPid = -1;	
	e->fileReadByte = 0;
	e->fileWroteByte = 0;
	e->wrote = 0;
	e->readByte = 0;
	registerClientSocketReadEvent(e);
}

void EventLoop::unregisterPipeWriteEvent(Event *e)
{
	EV_SET(&(dummyEvent), e->PtoCPipe[1], EVFILT_WRITE, EV_DELETE | EV_DISABLE, 0, 0, e);
	(kevent(this->kq_fd, &(dummyEvent), 1, NULL, 0, NULL)); 
	close(e->PtoCPipe[1]);
}

void EventLoop::unregisterFileWriteEvent(Event *e)
{
	EV_SET(&(dummyEvent), e->file_fd, EVFILT_WRITE, EV_DELETE | EV_DISABLE, 0, 0, e);
	(kevent(this->kq_fd, &(dummyEvent), 1, NULL, 0, NULL)); 
	close(e->file_fd);
}


void EventLoop::unregisterTmpFileReadEvent(Event *e)
{
	EV_SET(&(dummyEvent), e->tmpInFile, EVFILT_READ, EV_DELETE | EV_DISABLE, 0, 0, e);
	(kevent(this->kq_fd, &(dummyEvent), 1, NULL, 0, NULL)); 
	close(e->tmpInFile);
	close(e->oldStdOut);
	close(e->oldStdin);
	unlink(e->tmpInFileName.c_str());
	unlink(e->tmpOutFileName.c_str());
}

void setEnv(Event *e)
{
	HttpreqHandler *reqHandler = static_cast<HttpreqHandler *>(e->getRequestHandler());
	std::string tmp;
	std::cout<<e->getRoute()<<std::endl;
	e->getCgiEnv()[0] = strdup("AUTH_TYPE=Basic");
	e->getCgiEnv()[1] = strdup(("CONTENT_LENGTH="+ reqHandler->getRequestInfo().contentLength).c_str());
	e->getCgiEnv()[2] = strdup(("CONTENT_TYPE=" + reqHandler->getRequestInfo().contentType).c_str());
	e->getCgiEnv()[3] = strdup("GATEWAY_INTERFACE=CGI/1.1");
	e->getCgiEnv()[4] = strdup(("PATH_INFO=" + reqHandler->getRequestInfo().path).c_str());
	e->getCgiEnv()[5] = strdup(("PATH_TRANSLATED=" + e->getRoute()).c_str());
	e->getCgiEnv()[6] = strdup(("QUERY_STRING=" + reqHandler->getRequestInfo().queryParam).c_str());
	tmp = inet_ntoa(e->getSocketInfo().socket_addr.sin_addr);
	e->getCgiEnv()[7] = strdup(("REMOTE_ADDR=" + tmp).c_str());
	e->getCgiEnv()[8] = strdup("REMOTE_HOST=");
	e->getCgiEnv()[9] = strdup("REMOTE_IDENT=");
	e->getCgiEnv()[10] = strdup("REMOTE_USER=");
	e->getCgiEnv()[11] = strdup(("REQUEST_METHOD=" + reqHandler->getRequestInfo().method).c_str());
	e->getCgiEnv()[12] = strdup(("REQUEST_URI=" + reqHandler->getRequestInfo().path).c_str());
	e->getCgiEnv()[13] = strdup(("SCRIPT_NAME=" + reqHandler->getRequestInfo().path).c_str());
	e->getCgiEnv()[14] = strdup("SERVER_NAME=cgi");
	char pt[10];
    sprintf(pt, "%d", e->getDefaultServerData()->getListen());
	tmp = pt;
	e->getCgiEnv()[15] = strdup(("SERVER_PORT=" + tmp).c_str());
	e->getCgiEnv()[16] = strdup("SERVER_PROTOCOL=HTTP/1.1");
	e->getCgiEnv()[17] = strdup("SERVER_SOFTWARE=webserv/1.0");
	if (reqHandler->getRequestInfo().reqHeaderMap.find("X-Secret-Header-For-Test") != reqHandler->getRequestInfo().reqHeaderMap.end())
	{
		e->getCgiEnv()[18] = strdup(("HTTP_X_SECRET_HEADER_FOR_TEST=" + reqHandler->getRequestInfo().reqHeaderMap.find("X-Secret-Header-For-Test")->second).c_str());
		e->getCgiEnv()[19] = NULL;
	}
	else 
	{
		e->getCgiEnv()[18] = NULL;
	}
}

void EventLoop::unregisterTmpFileWriteEvent(Event *e)
{
	responseHandler *resHandler = static_cast<responseHandler *>(e->getResponseHandler());
	HttpreqHandler *reqHandler = static_cast<HttpreqHandler *>(e->getRequestHandler());
	EV_SET(&(dummyEvent), e->tmpOutFile, EVFILT_WRITE, EV_DELETE | EV_DISABLE, 0, 0, e);
	(kevent(this->kq_fd, &(dummyEvent), 1, NULL, 0, NULL)); 

	//FD백업
	e->oldStdin = dup(STDIN_FILENO);
	e->oldStdOut = dup(STDOUT_FILENO);

	//FD작업
	close(e->tmpOutFile);
	close(e->tmpInFile);

	std::cerr<<"exec cgi\n";
	pid_t pid;
	if ((pid = fork())  == -1)
	{
		e->setStatusCode(500);
		std::cerr << "fork error" << std::endl;
		return;
	}

	/**
	 * parent process
	 * */
	if (pid)
	{
		/* close(e->tmpOutFile); */
		/* close(e->tmpInFile); */
		e->childPid = pid;
		//reserve
		resHandler->getResBody().reserve(reqHandler->getRequestInfo().body.length());
		registerCgiExitEvent(e);
		return;
	}
	/**
	 * child process
	 * */
	else 
	{
		setEnv(e);
		std::cerr<<"tmpOutFile: "<<e->tmpOutFileName<<std::endl;
		std::cerr<<"tmpInFile: "<<e->tmpInFileName<<std::endl;
		if ((e->tmpOutFile = open(e->tmpOutFileName.c_str(), O_RDONLY)) == -1)
		{
			close(e->tmpOutFile);
			close(e->tmpInFile);
			std::cerr<<"open error"<<errno<<std::endl;
			exit(1);
		}
		if ((e->tmpInFile = open(e->tmpInFileName.c_str(), O_WRONLY)) == -1)
		{
			close(e->tmpInFile);
			close(e->tmpOutFile);
			std::cerr<<"open error"<<errno<<std::endl;
			exit(1);
		}
		int flag;
		flag = fcntl(e->tmpOutFile, F_GETFL, 0);
		flag |= O_NONBLOCK;
		if (fcntl(e->tmpOutFile, F_SETFL, flag) == -1)
			std::cerr<<"child fcntl error"<<errno<<std::endl;
		flag = fcntl(e->tmpInFile, F_GETFL, 0);
		flag |= O_NONBLOCK;
		if (fcntl(e->tmpInFile, F_SETFL, flag) == -1)
			std::cerr<<"child fcntl error"<<errno<<std::endl;

        if (dup2(e->tmpInFile, STDOUT_FILENO) == -1)
			std::cerr<<"dup2 error"<<errno<<std::endl;
        if (dup2(e->tmpOutFile, STDIN_FILENO) == -1)
			std::cerr<<"dup2 error"<<errno<<std::endl;

		//실행
		char **env = new char*[e->getCgiEnv().size() + 1];
		for (size_t i = 0; i < e->getCgiEnv().size(); i++)
			env[i] = const_cast<char *>(e->getCgiEnv()[i]);
		env[e->getCgiEnv().size()] = NULL;
		if (execve(e->getRoute().c_str(), NULL, env) == -1)
		{
			close(e->tmpInFile);
			close(e->tmpOutFile);
			std::cerr << "execve error" << std::endl;
			e->setStatusCode(404);
			exit(1);
		}
		close(e->tmpInFile);
		close(e->tmpOutFile);
		exit(1);
	}
}
