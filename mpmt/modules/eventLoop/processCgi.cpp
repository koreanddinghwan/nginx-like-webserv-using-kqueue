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

/* env[0] = strdup(("AUTH_TYPE=" + req->getReqHeaderValue("Authorization")).c_str()); */
/* 	env[1] = strdup(("CONTENT_LENGTH=" + req->getReqHeaderValue("Content-Length")).c_str());  // 요청 길이를 알 수 없는경우 -1 이여야함 */
/* 	env[2] = strdup(("CONTENT_TYPE=" + req->getReqHeaderValue("Content-Type")).c_str()); */
/* 	env[3] = strdup("GATEWAY_INTERFACE=CGI/1.1"); */
	/* env[4] = strdup(("PATH_INFO=" + req->getReqTarget()).c_str()); */
/* 	//env[5] = strdup(("PATH_TRANSLATED=");	// PATH_INFO의 변환. 스크립트의 가상경로를, 실제 호출 할 때 사용되는 경로로 맵핑. 요청 URI의 PATH_INFO 구성요소를 가져와, 적합한 가상 : 실제 변환을 수행하여 맵핑. */
/* 	//env[6] = strdup("QUERY_STRING=");	// 경로 뒤의 요청 URL에 포함된 조회 문자열. */
/* 	//env[7] = strdup("REMOTE_ADDR=");	// 요청을 보낸 클라이언트 IP 주소. */
/* 	//env[8] = strdup("REMOTE_IDENT=");	// Identification. 클라이언트에서 GCI 프로그램을 실행시킨 사용자. */
/* 	//env[9] = strdup("REMOTE_USER=");	// 사용자가 인증된 경우 이 요청을 작성한 사용자의 로그인을 의미.	null (인증되지 않음) */
/* 	env[5] = strdup(("REQUEST_METHOD=" + req->getMethod()).c_str());	// 요청 HTTP 메소드 이름. (GET, POST, PUT) */
/* 	env[6] = strdup(("REQUEST_URI=" + req->getReqTarget()).c_str());	// 현재 페이지 주소에서 도메인을 제외한 값. */
/* 	env[7] = strdup("SCRIPT_NAME=cgi");	// HTTP 요청의 첫 번째 라인에 있는 조회 문자열까지의 URL. */
/* 	env[8] = strdup("SERVER_NAME=webserv");	// 요청을 수신한 서버의 호스트 이름. */
/* 	//env[9] = strdup("SERVER_PORT=4242");	// 요청을 수신한 서버의 포트 번호. */
/* 	env[9] = strdup("SERVER_PROTOCOL=HTTP/1.1");	// 요청이 사용하는 프로토콜의 이름과 버전. 	protocol/majorVersion.minorVersion 양식 */
/* 	//env[16] = strdup("SERVER_SOFTWARE=");	// 서블릿이 실행 중인 컨테이너의 이름과 버전. */
/* 	env[10] = 0; */

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
