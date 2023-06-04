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
		std::cout << "fcntl error" << std::endl;
		return false;
	}
	if (fcntl(e->CtoPPipe[1], F_SETFL, O_NONBLOCK) == -1)
	{
		close(e->CtoPPipe[0]);
		close(e->CtoPPipe[1]);
		close(e->PtoCPipe[0]);
		close(e->PtoCPipe[1]);
		e->setStatusCode(500);
		std::cout << "fcntl error" << std::endl;
		return false;
	}
	if (fcntl(e->PtoCPipe[0], F_SETFL, O_NONBLOCK) == -1)
	{
		close(e->CtoPPipe[0]);
		close(e->CtoPPipe[1]);
		close(e->PtoCPipe[0]);
		close(e->PtoCPipe[1]);
		e->setStatusCode(500);
		std::cout << "fcntl error" << std::endl;
		return false;
	}
	if (fcntl(e->PtoCPipe[1], F_SETFL, O_NONBLOCK) == -1)
	{
		close(e->CtoPPipe[0]);
		close(e->CtoPPipe[1]);
		close(e->PtoCPipe[0]);
		close(e->PtoCPipe[1]);
		e->setStatusCode(500);
		std::cout << "fcntl error" << std::endl;
		return false;
	}
	return true;
}

void setEnv(Event *e)
{
	HttpreqHandler *reqHandler = static_cast<HttpreqHandler *>(e->getRequestHandler());
	e->getCgiEnv()[0]->append("AUTH_TYPE=Basic");
	e->getCgiEnv()[1]->append("CONTENT_LENGTH=");
	e->getCgiEnv()[1]->append(reqHandler->getRequestInfo().contentLength);
	e->getCgiEnv()[2]->append("CONTENT_TYPE=").append(reqHandler->getRequestInfo().contentType);
	e->getCgiEnv()[3]->append("GATEWAY_INTERFACE=CGI/1.1");
	e->getCgiEnv()[4]->append("PATH_INFO=").append("/YoupiBanane/youpi.bla");
	e->getCgiEnv()[5]->append("PATH_TRANSLATED=").append(e->getRoute());
	e->getCgiEnv()[6]->append("QUERY_STRING=").append(reqHandler->getRequestInfo().queryParam);
	e->getCgiEnv()[7]->append("REMOTE_ADDR=").append(inet_ntoa(e->getSocketInfo().socket_addr.sin_addr));
	e->getCgiEnv()[8]->append("REMOTE_HOST=");
	e->getCgiEnv()[9]->append("REMOTE_IDENT=");
	e->getCgiEnv()[10]->append("REMOTE_USER=");
	e->getCgiEnv()[11]->append("REQUEST_METHOD=").append(reqHandler->getRequestInfo().method);
	/* e->getCgiEnv()[12]->append("SCRIPT_NAME=").append(e->getDir().append(e->locationData->getCgiPass())); */
	e->getCgiEnv()[13]->append("SERVER_NAME=").append(e->serverName);
	char pt[10];
    sprintf(pt, "%d", e->getDefaultServerData()->getListen());
	e->getCgiEnv()[14]->append("SERVER_PORT=").append(pt);
	e->getCgiEnv()[15]->append("SERVER_PROTOCOL=").append(reqHandler->getRequestInfo().httpVersion);
	e->getCgiEnv()[16]->append("SERVER_SOFTWARE=webserv/1.0");
	e->getCgiEnv()[17]->append("scheme=http");
	e->getCgiEnv()[18]->append("protocol-var-name=HTTP/1.1");
	e->getCgiEnv()[19]->append("extension-var-name=");
}

bool EventLoop::processCgi(Event *e)
{
	/**
	 * first, check if the cgi file exists
	 * */
	e->setRoute(e->locationData->getRoot() + e->locationData->getCgiPass());
	if (stat(e->getRoute().c_str(), &e->statBuf) != 0)
	{
		std::cout << "stat error" << std::endl;
		e->setStatusCode(404);
		return false;
	}


	HttpreqHandler *reqHandler = static_cast<HttpreqHandler *>(e->getRequestHandler());
	setEnv(e);

	/**
	 * 1. create pipe
	 * */
	if (pipe(e->CtoPPipe) == -1)
	{
		e->setStatusCode(500);
		std::cout << "pipe error" << std::endl;
		return false;
	}
	if (pipe(e->PtoCPipe) == -1)
	{
		close(e->CtoPPipe[0]);
		close(e->CtoPPipe[1]);
		e->setStatusCode(500);
		std::cout << "pipe error" << std::endl;
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
		std::cout << "fork error" << std::endl;
		return false;
	}


	/**
	 * 4. parent process
	 * */
	if (pid)
	{
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
		close(e->CtoPPipe[0]);
		close(e->PtoCPipe[1]);

		// 표준 출력을 CtoP파이프의 쓰기용 파일 디스크립터로 리디렉션
        dup2(e->CtoPPipe[1], STDOUT_FILENO);
		// 표준 입력을 PtoC파이프의 읽기용 파일 디스크립터로 리디렉션해서 부모 프로세스로부터 데이터를 읽어들임
        dup2(e->PtoCPipe[0], STDIN_FILENO);

		//실행
		char **env = new char*[e->getCgiEnv().size() + 1];
		for (size_t i = 0; i < e->getCgiEnv().size(); i++)
			env[i] = const_cast<char *>(e->getCgiEnv()[i]->c_str());
		env[e->getCgiEnv().size()] = NULL;
		if (execve(e->getRoute().c_str(), NULL, env) == -1)
		{
			e->setStatusCode(404);
			exit(1);
		}
		exit(1);
	}
}
