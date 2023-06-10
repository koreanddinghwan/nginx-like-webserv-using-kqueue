#include "EventLoop.hpp"
#include <cstdio>
#include <cstdlib>
#include <sys/fcntl.h>
#include <unistd.h>

bool EventLoop::processCgi(Event *e)
{
	std::cout << "processCgi" << std::endl;
	std::cout<<"file des:"<<e->getClientFd()<<std::endl;

	if (static_cast<HttpreqHandler *>(e->getRequestHandler())->getCurrentBodyLength() == 0)
	{
		e->setStatusCode(200);
		return false;
	}




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
	e->setTmpInPath();
	e->setTmpOutPath();

	if ((e->tmpOutFile = open(e->tmpOutFileName.c_str(), O_CREAT | O_WRONLY | O_TRUNC | O_NONBLOCK, 0644 )) == -1)
		std::cout<<"register tmp file write event error"<<std::endl;
	if ((e->tmpInFile = open(e->tmpInFileName.c_str(), O_CREAT | O_RDONLY | O_TRUNC | O_NONBLOCK, 0644) == -1))
		std::cout<<"register tmp file read event error"<<std::endl;
	std::cout<<"tmp out path: "<<e->tmpOutFileName<<"fd :" << e->tmpOutFile<<std::endl;
	if (fcntl(e->tmpOutFile, F_SETFL, O_NONBLOCK) == -1)
		std::cout<<"error fcntl"<<e->tmpOutFileName<< errno<<std::endl;
	if (fcntl(e->tmpInFile, F_SETFL, O_NONBLOCK) == -1)
		std::cout<<"error fcntl"<<e->tmpOutFileName<< errno<<std::endl;
	registerTmpFileWriteEvent(e);
	return true;
}
