#include "../eventLoop/EventLoop.hpp"
#include <cstdint>
#include <exception>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include "../http/HttpReqHandler.hpp"
#include "Event.hpp"


int getLongestPrefixMatchScore(const std::string& location, const std::string& requestPath) {
	if (location.back() == '/')
	{
		if (location == requestPath)
			return INT32_MAX;
	}
    size_t length = std::min(location.length(), requestPath.length());

	/**
	 * @TODO 
	 * if location uri ends with /
	 * */
    // Find the index where the location and requestPath differ
    size_t index = 0;
    while (index < length && location[index] == requestPath[index]) {
        index++;
    }
    // Return the matched prefix length
    return location.substr(0, index).length();
}


void setServerName(Event *e)
{
	HttpreqHandler *reqHandler = static_cast<HttpreqHandler *>(e->getRequestHandler());


	/**
	 * 1. check host name in server names
	 * */
	// 효율 안좋다 ..
	std::string host = reqHandler->getRequestInfo().host;
	for (int i = 0; i < e->getServerData()->size(); i++)
	{
		std::vector<std::string>& serverNames = (e->getServerData()->at(i)->getServerNames());
		for (int j = 0; j < serverNames.size(); j++)
		{
			//i번째 serverData에 있는 server names에서 host가 있으면
			if (host == serverNames[j])
			{
				//사용할 serverData를 i번째로 바꿔준다.
				e->setDefaultServerData(e->getServerData()->at(i));
				e->serverName = serverNames[j];
				break ;
			}
		}
	}
}

void setLocationData(Event *e) throw (std::exception)
{
	HttpreqHandler *reqHandler = static_cast<HttpreqHandler *>(e->getRequestHandler());
	/**
	 * 2. check the host's uri in location datas by prefix matching
	 * */
	std::string requestPath = e->internal_uri;
	e->locationData = NULL;
	int matchScore = -1;
	for (int i = 0; i < e->getDefaultServerData()->getLocationDatas().size(); i++)
	{
		std::string &locationUri = e->getDefaultServerData()->getLocationDatas().at(i)->getUri();
		if (matchScore < getLongestPrefixMatchScore(locationUri, requestPath))
		{
			matchScore = getLongestPrefixMatchScore(locationUri, requestPath);
			e->locationData = e->getDefaultServerData()->getLocationDatas().at(i);
		}
	}
	if (!e->locationData)
	{
		std::cout<<"no location data"<<std::endl;
		e->setStatusCode(404);
		throw std::exception();
	}
	std::cout<<"============location setted============="<<std::endl;
	e->locationData->printLocationData();
}

void checkAllowedMethods(Event *e) throw (std::exception)
{
	HttpreqHandler *reqHandler = static_cast<HttpreqHandler *>(e->getRequestHandler());
	int methodIndex = MethodFactory::getInstance().getMethodIndex(e->internal_method);
	if (!(e->locationData->getLimitedMethods().methods[methodIndex]))
	{
		/**
		 * 405: "method not allowed"
		 **/
		e->setStatusCode(405);
		/**
		 * throw exception.
		 * if event loop catch this exception, then the httpexception event will be registered.
		 * */
		throw std::exception();
	}
}

void checkClientMaxBodySize(Event *e) throw(std::exception)
{
	HttpreqHandler *reqHandler = static_cast<HttpreqHandler *>(e->getRequestHandler());
	if (e->locationData->getClientMaxBodySize() < reqHandler->getRequestInfo().body.length())
	{
		e->setStatusCode(413);
		throw std::exception();
	}
}

void setRoute(Event *e)
{
	std::string requestPath = e->internal_uri;
	int pos;
	std::string tmp;

	if (e->locationData->getUri() == "/")
	{
		e->setResource(requestPath);
		e->setRoute(e->locationData->getRoot() + e->getResource());
	}

	// 1. check if the requested resource is directory
	else if (requestPath.back() == '/')
	{
		if (requestPath.length() == 1)
		{
			e->setResource("/");
			e->setRoute(e->locationData->getRoot() + e->getResource());
		}
		else
		{
			if (e->locationData->getUri() == requestPath)
			{
				/**
				 * path : /test/
				 * loc  : /test/
				 * resource : /
				 * route : root + /
				 * */
				e->setResource("/");
				e->setRoute(e->locationData->getRoot() + e->getResource());
			}
			else if (e->locationData->getUri().back() == '/')
			{
				/**
				 * request path is directory and location uri is directory
				 * */
				/**
				 * path : /test/
				 * loc  : /
				 * resource : test/
				 * route : root + / + test/
				 * */
				/**
				 * path : /test/abc/
				 * loc  : /test/
				 * resource : abc/
				 * route : root + / + abc/
				 * */
				tmp = requestPath.substr(1);
				pos = tmp.find("/");
				e->setResource(requestPath.substr(pos + 1));
				e->setRoute(e->locationData->getRoot() + "/" + e->getResource());
			}

			else {
				/**
				 * request path is directory but location uri is not directory
				 * */
				/**
				 * path : /test/
				 * loc  : /t
				 * resource : /
				 * route : root + /
				 * */
				/**
				 * path : /test/
				 * loc  : /test
				 * resource : /
				 * route : root + /
				 * */
				/**
				 * path : /test/abc/
				 * loc  : /testasdfasdfa
				 * resource : /abc/
				 * route : root + /abc//
				 * */
				tmp = requestPath.substr(1);
				std::cout<<tmp<<std::endl;
				pos = tmp.find("/");
				e->setResource(requestPath.substr(pos));
				e->setRoute(e->locationData->getRoot() + e->getResource());
			}
		}
	}
	// 2. check if the requested resource is file
	else
	{
		/**
		 * request path is file and location uri is not directory
		 * */
		if (e->locationData->getUri().back() != '/')
		{
			if (e->locationData->getUri() == requestPath)
			{
				/**
				 * request path and location uri is equal
				 * */
				/**
				 * path : /test
				 * loc  : /test
				 * resource : ""
				 * route : root + ""
				 * */
				e->setResource("");
				e->setRoute(e->locationData->getRoot() + e->getResource());
			}
			else if (
					requestPath.substr(1)\
					.find("/") == std::string::npos)
			{
				/**
				 * resource identifier not exists
				 * */
				/**
				 * path : /test
				 * loc  : /t
				 * resource : ""
				 * route : root + ""
				 * */
				e->setResource("");
				e->setRoute(e->locationData->getRoot() + e->getResource());
			}
			else {
				/**
				 * resource identifier exists
				 * */
				/**
				 * path : /test/abc
				 * loc  : /tes
				 * resource : abc
				 * route : root + / + abc
				 * */
				/**
				 * path : /test/abc
				 * loc  : /test
				 * resource : abc
				 * route : root + / + abc
				 * */
				tmp = requestPath.substr(1);
				pos = tmp.find("/");
				e->setResource(tmp.substr(pos + 1));
				e->setRoute(e->locationData->getRoot() + "/" + e->getResource());
			}
		}
		else 
		{
			/**
			 * request path is file but location uri is directory
			 * */
			/**
			 * path : /test/abc
			 * loc  : /test/
			 * resource : abc
			 * route : root + "/" + abc
			 * */
			/**
			 * path : /test/abc
			 * loc  : /
			 * resource : test/abc
			 * route : root + test/abc
			 * */
			tmp = requestPath.substr(1);
			pos = tmp.find("/");
			e->setResource(tmp.substr(pos + 1));
			e->setRoute(e->locationData->getRoot() + "/" + e->getResource());
		}
	}
	std::cout<<"=======!!!!!!!!!!!!!!!!!!!!!!!!!11!!!!=====resource setted============="<<std::endl;
	std::cout<<"route: "<<e->getRoute()<<std::endl;
	std::cout<<"resource: "<<e->getResource()<<std::endl;
	std::cout<<"=======!!!!!!!!!!!!!!!!!!!!!!!!!!!1================================"<<std::endl;
}

void setRedirection(Event *e) throw(std::exception)
{
	if (!e->locationData->getRedirectUrl().empty())
	{
		e->setStatusCode(e->locationData->getReturnStatus());
		throw std::exception();
	}
}


/**
 * handle error
 *
 * 1. check host name in server names
 * 	-> and choose server data to use
 * 2. set the location data to use by checking the host's uri in location datas by prefix matching 
 * 3. check the allowed_mtehods in location data
 * 4. make resource route in server with root route
 * 5. if redirecturl exists, redirect to url
 * 		-> set status code and redirection url.
 * */
void EventLoop::setHttpResponse(Event *e)
{
	HttpreqHandler *reqHandler = static_cast<HttpreqHandler *>(e->getRequestHandler());

	/**
	 * 1. check host name in server names
	 * */
	setServerName(e);

	/**
	 * 2. check the host's uri in location datas by prefix matching
	 * */
	setLocationData(e);

	/**
	 * 3. check the allowed_methods in location data
	 * */
	checkAllowedMethods(e);

	/**
	 * 4. check clientMaxBodySize
	 * */
	checkClientMaxBodySize(e);

	/**
	 * 5. make resource route in server with root route
	 * */
	setRoute(e);

	/**
	 * 6. set errorpage
	 * */

	/**
	 * 7. if redirecturl exists, redirect to url
	 * */
	setRedirection(e);

	/**
	 * 8. if need cgi process
	 * */
	if (!e->locationData->getCgiPass().empty())
	{
		/**
		 * process cgi
		 * */
		//process cgi
		if (!processCgi(e))
			throw std::exception();
		return ;
	}

	/**
	 * 9. process methods
	 * */
	int methodIndex = MethodFactory::getInstance().getMethodIndex(reqHandler->getRequestInfo().method);
	/**
	 * if method == GET
	 * */
	if (methodIndex == GET || methodIndex == HEAD)
		ws_method_GET(e);
	else if (methodIndex == POST)
		ws_method_POST(e);
	else if (methodIndex == DELETE)
		ws_method_DELETE(e);
	e->setStatusCode(404);
	throw std::exception();
}
