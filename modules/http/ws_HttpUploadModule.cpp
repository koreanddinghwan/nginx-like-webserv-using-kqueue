#include "ws_HttpUploadModule.hpp"
#include <sys/fcntl.h>


bool ws_HttpUploadModule::processEvent(Event *e)
{
	HttpreqHandler *h = static_cast<HttpreqHandler *>(e->getRequestHandler());

	if (h->getRequestInfo().body.length() == 0)
	{
		e->setStatusCode(204);
		return false;
	}

	/**
	 * POST method do not allow create directory
	 * */
	if (e->internal_uri == "/")
	{
		// 400? 403?
		e->setStatusCode(204);
		return false;
	}


	std::string filePath;
	if (e->internal_uri[0] == '/')
		filePath = e->locationData->getRoot() + "/" + e->locationData->getUploadStore() + e->internal_uri;
	else
		filePath = e->locationData->getRoot() + "/" + e->locationData->getUploadStore() + "/" + e->internal_uri;
	if (filePath.back() == '/')
	{
		e->setStatusCode(204);
		return false;
	}
	if (stat(filePath.c_str(), &e->statBuf) == 0 &&
			e->statBuf.st_mode & S_IFDIR)
	{
		e->setStatusCode(204);
		return false;
	}
	if((e->file_fd = open(filePath.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0644)) == -1)
	{
		e->setStatusCode(500);
		return false;
	}

	if ((fcntl(e->file_fd, F_SETFL, O_NONBLOCK)) == -1)
	{
		e->setStatusCode(500);
		return false;
	}
	e->setStatusCode(201);
	return true;
}
