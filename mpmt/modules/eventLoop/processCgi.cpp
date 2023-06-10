#include "EventLoop.hpp"
#include <cstdio>
#include <cstdlib>
#include <sys/fcntl.h>
#include <unistd.h>

bool EventLoop::processCgi(Event *e)
{
	std::cerr << "processCgi" << std::endl;
	std::cerr<<"file des:"<<e->getClientFd()<<std::endl;

	if (static_cast<HttpreqHandler *>(e->getRequestHandler())->getCurrentBodyLength() == 0)
	{
		e->setStatusCode(204);
		return false;
	}

	/**
	 * first, check if the cgi file exists
	 * */
	e->setRoute(e->locationData->getRoot() + e->locationData->getCgiPass());
	if (stat(e->getRoute().c_str(), &e->statBuf) != 0)
	{
		std::cerr << "stat error" << std::endl;
		e->setStatusCode(404);
		return false;
	}
	e->setTmpInPath();
	e->setTmpOutPath();


		close(e->tmpOutFile);
		close(e->tmpInFile);
	if ((e->tmpOutFile = open(e->tmpOutFileName.c_str(), O_CREAT | O_RDWR | O_TRUNC, 0644 )) == -1)
		std::cerr<<"register tmp file write event error"<<std::endl;
	if ((e->tmpInFile = open(e->tmpInFileName.c_str(), O_CREAT | O_RDWR | O_TRUNC, 0666) == -1))
		std::cerr<<"register tmp file read event error"<<std::endl;

	int flag;
	flag = fcntl(e->tmpOutFile, F_GETFL, 0);
	flag |= O_NONBLOCK;
	if (fcntl(e->tmpOutFile, F_SETFL, flag) == -1)
		std::cerr<<"parent fcntl error"<<errno<<std::endl;
	flag = fcntl(e->tmpInFile, F_GETFL, 0);
	flag |= O_NONBLOCK;
	if (fcntl(e->tmpInFile, F_SETFL, flag) == -1)
		std::cerr<<"parent fcntl error"<<errno<<std::endl;
	registerTmpFileWriteEvent(e);
	return true;
}
