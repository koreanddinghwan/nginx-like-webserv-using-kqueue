#include "./ws_HttpAutoIndexModule.hpp"
#include <dirent.h>
#include <stdlib.h>
#include <string>
#include <sys/stat.h>
#include <vector>
#include "HttpReqHandler.hpp"
#include "responseHandler.hpp"

bool ws_HttpAutoIndexModule::processEvent(Event *e)
{
	/**
	 *
	 * check requested 
	 * */

	std::vector<std::string> list;

	e->setRoute(e->locationData->getRoot() + e->internal_uri);
	std::cout<<e->getRoute()<<std::endl;
	DIR *dir = opendir(e->getRoute().c_str());
	if (dir)
	{
		struct dirent *ent;
		std::string path;

		//read dir
		while ((ent = readdir(dir)) != NULL)
		{
			if (ent->d_name[0] == '.')
			{
				std::string tmp = ent->d_name;
				list.push_back(tmp +  "/");
			}
			else {
				path =  e->internal_uri;
				path.back() == '/' ? path = path  + ent->d_name  :  path = path + "/" + ent->d_name;
				if (path.front() == '/')
					path = path.substr(1);
				stat((e->getRoute() + ent->d_name).c_str(), &(e->statBuf));
				if (S_ISDIR(e->statBuf.st_mode))
					list.push_back(path + std::string("/"));
				else
					list.push_back(path);
			}
		}
		closedir(dir);

		/**
		 * make html response body
		 * */
		responseHandler *resHandler = static_cast<responseHandler *>(e->getResponseHandler());
		HttpreqHandler *reqHandler = static_cast<HttpreqHandler *>(e->getRequestHandler());

		resHandler->setResBody("<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\" /><meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\" /><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" /><title>");
		resHandler->setResBody(e->getRoute());
		resHandler->setResBody("</title></head><body><h1>Index of ");
		resHandler->setResBody(e->getRoute());
		resHandler->setResBody("</h1><hr />");
		for (int i = 0; i < list.size(); i++)
		{
			resHandler->setResBody("<a href=http://");
			resHandler->setResBody(reqHandler->getRequestInfo().host + "/");
			resHandler->setResBody(list[i]);
			resHandler->setResBody(">");
			resHandler->setResBody(list[i]);
			resHandler->setResBody("</a><br />");
		}
		resHandler->setResBody("<hr /></body></html>");
		e->setStatusCode(200);
		return true;
	}
	e->setStatusCode(404);
	return false;
}
