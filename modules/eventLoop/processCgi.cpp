#include "EventLoop.hpp"

bool setFcntlToPipe(Event *e)
{
	if (fcntl(e->CtoPPipe[0], F_SETFL, O_NONBLOCK) == -1)
	{
		close(e->CtoPPipe[0]);
		close(e->CtoPPipe[1]);
		close(e->PtoCPipe[0]);
		close(e->PtoCPipe[1]);
		e->setStatusCode(500);
		return false;
	}
	if (fcntl(e->CtoPPipe[1], F_SETFL, O_NONBLOCK) == -1)
	{
		close(e->CtoPPipe[0]);
		close(e->CtoPPipe[1]);
		close(e->PtoCPipe[0]);
		close(e->PtoCPipe[1]);
		e->setStatusCode(500);
		return false;
	}
	if (fcntl(e->PtoCPipe[0], F_SETFL, O_NONBLOCK) == -1)
	{
		close(e->CtoPPipe[0]);
		close(e->CtoPPipe[1]);
		close(e->PtoCPipe[0]);
		close(e->PtoCPipe[1]);
		e->setStatusCode(500);
		return false;
	}
	if (fcntl(e->PtoCPipe[1], F_SETFL, O_NONBLOCK) == -1)
	{
		close(e->CtoPPipe[0]);
		close(e->CtoPPipe[1]);
		close(e->PtoCPipe[0]);
		close(e->PtoCPipe[1]);
		e->setStatusCode(500);
		return false;
	}
	return true;
}

void setEnv(Event *e)
{
	HttpreqHandler *reqHandler = static_cast<HttpreqHandler *>(e->getRequestHandler());
	std::string tmp;
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
	e->getCgiEnv()[14] = strdup(("SERVER_PORT=" + tmp).c_str());
	e->getCgiEnv()[15] = strdup("SERVER_PROTOCOL=HTTP/1.1");
	e->getCgiEnv()[16] = strdup("SERVER_SOFTWARE=webserv/1.0");
	if (reqHandler->getRequestInfo().reqHeaderMap.find("X-Secret-Header-For-Test") != reqHandler->getRequestInfo().reqHeaderMap.end())
	{
		e->getCgiEnv()[17] = strdup(("HTTP_X_SECRET_HEADER_FOR_TEST=" + reqHandler->getRequestInfo().reqHeaderMap.find("X-Secret-Header-For-Test")->second).c_str());
		e->getCgiEnv()[18] = NULL;
	}
	else 
	{
		e->getCgiEnv()[17] = NULL;
	}
}

bool EventLoop::processCgi(Event *e)
{
	/**
	 * first, check if the cgi file exists
	 * */
	e->setRoute(e->locationData->getRoot() + e->locationData->getCgiPass());
	if (stat(e->getRoute().c_str(), &e->statBuf) != 0)
	{
		e->setStatusCode(404);
		return false;
	}


	HttpreqHandler *reqHandler = static_cast<HttpreqHandler *>(e->getRequestHandler());

	/**
	 * 1. create pipe
	 * */
	if (pipe(e->CtoPPipe) == -1)
	{
		e->setStatusCode(500);
		return false;
	}
	if (pipe(e->PtoCPipe) == -1)
	{
		close(e->CtoPPipe[0]);
		close(e->CtoPPipe[1]);
		e->setStatusCode(500);
		return false;
	}

	/**
	 * 2. set non blocking to pipe
	 * */
	if (setFcntlToPipe(e) == false)
		return false;

	/**
	 * 3. fork
	 * */
	pid_t pid = fork();
	if (pid == -1)
	{
		close(e->CtoPPipe[0]);
		close(e->CtoPPipe[1]);
		close(e->PtoCPipe[0]);
		close(e->PtoCPipe[1]);
		e->setStatusCode(500);
		return false;
	}


	/**
	 * 4. parent process
	 * */
	if (pid)
	{
		e->setStatusCode(200);
		e->childPid = pid;
		close(e->CtoPPipe[1]);
		close(e->PtoCPipe[0]);

		e->setEventType(E_PIPE);

		//unregisterClientSocketReadEvent(e);
		unregisterClientSocketReadEvent(e);

		//read from CtoPPipe[0]
		registerPipeReadEvent(e);

		//write to PtoCPipe[1]
		registerPipeWriteEvent(e);
		return true;
	}
	/**
	 * 5. child process
	 * */
	else 
	{
		setEnv(e);
		close(e->CtoPPipe[0]);
		close(e->PtoCPipe[1]);

		// 표준 출력을 CtoP파이프의 쓰기용 파일 디스크립터로 리디렉션
        dup2(e->CtoPPipe[1], STDOUT_FILENO);
		// 표준 입력을 PtoC파이프의 읽기용 파일 디스크립터로 리디렉션해서 부모 프로세스로부터 데이터를 읽어들임
        dup2(e->PtoCPipe[0], STDIN_FILENO);

		//실행
		char **env = new char*[e->getCgiEnv().size() + 1];
		for (size_t i = 0; i < e->getCgiEnv().size(); i++)
			env[i] = const_cast<char *>(e->getCgiEnv()[i]);
		env[e->getCgiEnv().size()] = NULL;
		if (execve(e->getRoute().c_str(), NULL, env) == -1)
		{
			e->setStatusCode(404);
			exit(1);
		}
		exit(1);
	}
}
