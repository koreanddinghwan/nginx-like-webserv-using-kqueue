#include "./ws_HttpIndexModule.hpp"
#include <sys/fcntl.h>

bool ws_HttpIndexModule::processEvent(Event *e)
{
	for (size_t i = 0; i < e->locationData->getIndex().size(); i++)
	{
		e->openFileName = e->locationData->getIndex().at(i);
		//if index file start with /, absolute path
		if (e->openFileName[0] == '/')
		{
			/**
			 * 절대경로 index파일
			 * */
			//만약 있으면
			if (stat(e->openFileName.c_str(), &e->statBuf) == 0)
			{
				if (S_ISREG(e->statBuf.st_mode))
				{
					//set internal redirection route
					e->internal_method = "GET";
					e->internal_uri = e->openFileName;
					return true;
				}
			}
		}
		else
		{
			/**
			 * 상대경로 index파일 설정은 root에 붙여서 식별해야함.
			 * */
			std::string tmp = e->locationData->getRoot() + "/" + e->openFileName;
			if (stat(tmp.c_str(), &e->statBuf) == 0 &&
					S_ISREG(e->statBuf.st_mode))
			{
				//set internal redirection route
				e->internal_method = "GET";
				if(e->internal_uri == "")
					e->internal_uri = e->internal_uri + "/" + e->openFileName;
				else if (e->internal_uri.back() == '/')
					e->internal_uri = e->internal_uri + e->openFileName;
				else
					e->internal_uri = e->internal_uri + "/" + e->openFileName;
				return true;
			}
		}
	}
	return false;
}
