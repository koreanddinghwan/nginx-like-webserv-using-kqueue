#include "./ws_HttpAutoIndexModule.hpp"
#include <dirent.h>
#include <string>
#include <vector>
#include "responseHandler.hpp"

bool ws_HttpAutoIndexModule::processEvent(Event *e)
{
	/**
	 *
	 * check requested 
	 * */

	std::vector<std::string> list;

	DIR *dir = opendir(e->getRoute().c_str());
	if (dir)
	{
		struct dirent *ent;

		//read dir
		while ((ent = readdir(dir)) != NULL)
		{
			list.push_back(ent->d_name);
		}
		closedir(dir);

		/**
		 * make html response body
		 * */
		responseHandler *resHandler = static_cast<responseHandler *>(e->getResponseHandler());


		resHandler->setResBody("<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\" /><meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\" /><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" /><title>");
		resHandler->setResBody(e->getRoute());
		resHandler->setResBody("</title></head><body><h1>Index of ");
		resHandler->setResBody(e->getRoute());
		resHandler->setResBody("</h1><hr />");
		for (int i = 0; i < list.size(); i++)
		{
			resHandler->setResBody("<a href=./");
			resHandler->setResBody(list[i]);
			resHandler->setResBody("\">");
			resHandler->setResBody(list[i]);
			resHandler->setResBody("</a><br />");
		}
		resHandler->setResBody("<hr /></body></html>");
		e->setStatusCode(200);
	}


	e->setStatusCode(404);
	return false;
}
