#include "./ws_HttpIndexModule.hpp"
#include <sys/fcntl.h>

bool ws_HttpIndexModule::processEvent(Event *e)
{
	for (int i = 0; i < e->locationData->getIndex().size(); i++)
	{
		e->openFileName = e->locationData->getIndex().at(i);
		//if index file start with /, absolute path
		if (e->openFileName[0] == '/')
		{
			//만약 있으면
			if (stat(e->openFileName.c_str(), &e->statBuf) == 0)
			{
				//open file and file read event
				e->file_fd = open(e->openFileName.c_str(), O_RDONLY);
				if (e->file_fd == -1)
				{
					e->setStatusCode(404);
					return false;
				}

				//set nonblock
				if (fcntl(e->file_fd, F_SETFL, O_NONBLOCK) == -1)
				{
					e->setStatusCode(404);
					return false;
				}
				e->setStatusCode(200);
				return true;
			}
		}
		else
		{
			//else, relative path
			e->setRoute(e->getDir() + "/" + e->openFileName);
			if (stat(e->getRoute().c_str(), &e->statBuf) == 0)
			{
				e->setStatusCode(200);
				//set event
				return true;
			}
		}
	}
	e->setStatusCode(404);
	return false;
}
