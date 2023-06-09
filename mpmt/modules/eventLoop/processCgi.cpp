#include "EventLoop.hpp"
#include <cstdio>
#include <cstdlib>
#include <sys/fcntl.h>
#include <unistd.h>

bool EventLoop::processCgi(Event *e)
{
	std::cerr << "processCgi" << std::endl;
	std::cerr<<"file des:"<<e->getClientFd()<<std::endl;
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


	if ((e->tmpOutFile = open(e->tmpOutFileName.c_str(), O_CREAT | O_RDWR | O_TRUNC, 0644 )) == -1)
		std::cerr<<"register tmp file write event error"<<std::endl;
	if ((e->tmpInFile = open(e->tmpInFileName.c_str(), O_CREAT | O_RDWR | O_TRUNC, 0666) == -1))
		std::cerr<<"register tmp file read event error"<<std::endl;

	if (fcntl(e->tmpOutFile, F_SETFL, O_NONBLOCK) == -1)
		std::cerr<<"error fcntl"<<e->tmpOutFileName<< errno<<std::endl;
	if (fcntl(e->tmpInFile, F_SETFL, O_NONBLOCK) == -1)
		std::cerr<<"error fcntl"<<e->tmpOutFileName<< errno<<std::endl;
	registerTmpFileWriteEvent(e);
	return true;
}
