#include "./ws_HttpIndexModule.hpp"
#include <sys/fcntl.h>

bool ws_HttpIndexModule::processEvent(Event *e)
{
	for (int i = 0; i < e->locationData->getIndex().size(); i++)
	{
		e->openFileName = e->locationData->getIndex().at(i);
		//if index file start with /, absolute path
		std::cout<<"index file name: "<<e->openFileName<<std::endl;
		if (e->openFileName[0] == '/')
		{
			std::cout<<"absolute path"<<std::endl;
			//만약 있으면
			if ((stat(e->openFileName.c_str(), &e->statBuf) == 0) && 
					(e->file_fd = open(e->openFileName.c_str(), O_RDONLY)) != -1)
			{
				//set nonblock
				if (fcntl(e->file_fd, F_SETFL, O_NONBLOCK) == -1)
				{
					e->setStatusCode(500);
					return false;
				}
				e->setStatusCode(200);
				return true;
			}
			else
			{
				if (e->locationData->getAutoIndex() == false)
				{
					e->setStatusCode(404);
					return false;
				}
			}
		}
		else
		{
			std::cout<<"relative path"<<std::endl;
			std::string tmp = e->locationData->getRoot() + "/" + e->openFileName;

			if ((stat(tmp.c_str(), &e->statBuf) == 0) && 
						(e->file_fd = open(tmp.c_str(), O_RDONLY)) != -1)
			{
				if (fcntl(e->file_fd, F_SETFL, O_NONBLOCK) == -1)
				{
					e->setStatusCode(500);
					return false;
				}
				e->setStatusCode(200);
				return true;
			}
			else
			{
				if (e->locationData->getAutoIndex() == false)
				{
					e->setStatusCode(404);
					return false;
				}
			}
		}
	}
}
