#include "../eventLoop/EventLoop.hpp"
#include <cstdint>
#include <ctime>
#include <exception>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include "../http/HttpReqHandler.hpp"
#include "Event.hpp"


int getLongestPrefixMatchScore(const std::string& location, const std::string& requestPath) {

	if (location != "/" && location.back() == '/')
	{
		if (requestPath.find(location) != std::string::npos)
			return INT32_MAX;
	}

    size_t length = std::min(location.length(), requestPath.length());

    // Find the index where the location and requestPath differ
    size_t index = 0;
    while (index < length && location[index] == requestPath[index]) {
        index++;
    }
    // Return the matched prefix length
    return location.substr(0, index).length();
}


/**
 * @brief request의 host를 보고 server block을 선택합니다.
 */
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

/**
 * @brief location data를 선택합니다.
 *
 * longest prefix matching을 사용합니다.
 * @ref https://www.digitalocean.com/community/tutorials/nginx-location-directive
 * @param e
 *
 * @throw std::exception
 */
bool setLocationData(Event *e)
{
	HttpreqHandler *reqHandler = static_cast<HttpreqHandler *>(e->getRequestHandler());

	std::cout<<"============setting location data============="<<std::endl;

	std::string requestPath = e->internal_uri;
	e->locationData = NULL;
	int matchScore = -1;
	for (int i = 0; i < e->getDefaultServerData()->getLocationDatas().size(); i++)
	{
		std::string &locationUri = e->getDefaultServerData()->getLocationDatas().at(i)->getUri();

		//cgi pass확인
		if (!(e->getDefaultServerData()->getLocationDatas().at(i)->getCgiPass().empty()))
		{
			std::string ext = locationUri.substr(1);
			// request가 ext로 끝나면
			if (requestPath.find(ext) != std::string::npos &&
					requestPath.find(ext) == requestPath.length() - ext.length())
			{
				e->locationData = e->getDefaultServerData()->getLocationDatas().at(i);
				return true;
			}
		}
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
		return false;
	}
	std::cout<<"============location setted============="<<std::endl;
	e->locationData->printLocationData();
	return true;
}

/**
 * @brief 요청한 메서드가 허용된 메서드인지 확인합니다.
 *
 * @param e
 *
 * @throw std::exception
 */
bool checkAllowedMethods(Event *e) throw (std::exception)
{
	std::cout<<"check allowed methods"<<std::endl;
	std::cout<<"method:: "<<e->internal_method<<std::endl;

	HttpreqHandler *reqHandler = static_cast<HttpreqHandler *>(e->getRequestHandler());
	int methodIndex = MethodFactory::getInstance().getMethodIndex(e->internal_method);

	std::cout<<methodIndex<<std::endl;
	std::cout<<e->locationData->getLimitedMethods().methods[methodIndex]<<std::endl;
	if ((e->locationData->getLimitedMethods().methods[methodIndex]) == 0)
	{
		std::cout<<"method not allowed"<<std::endl;
		/**
		 * 405: "method not allowed"
		 **/
		e->setStatusCode(405);
		return false;
	}
	return true;
}

bool checkClientMaxBodySize(Event *e) throw(std::exception)
{
	HttpreqHandler *reqHandler = static_cast<HttpreqHandler *>(e->getRequestHandler());
	std::cout<<"check client max body size"<<std::endl;
	std::cout<<"body length = "<<reqHandler->getRequestInfo().body.length()<<std::endl;
	std::cout<<"client max body size = "<<e->locationData->getClientMaxBodySize()<<std::endl;
	if (e->locationData->getClientMaxBodySize() && (e->locationData->getClientMaxBodySize() < reqHandler->getRequestInfo().body.length()))
	{
		e->setStatusCode(413);
		return false;
	}
	return true;
}

void setInternalUri(Event *e)
{
	std::string requestPath = e->internal_uri;
	int pos;
	std::string tmp;
	std::cout<<"============setting Route=========="<<std::endl;
	std::cout<<"internal uri ="<<e->internal_uri<<std::endl;
	std::cout<<"location uri ="<<e->locationData->getUri()<<std::endl;
	std::cout<<"location root = "<<e->locationData->getRoot()<<std::endl;

	if (!e->locationData->getCgiPass().empty())
	{
		e->internal_uri = e->locationData->getCgiPass();
		return ;
	}


	tmp = requestPath.substr(1);
	pos = tmp.find("/");

	if (e->locationData->getUri() == "/")
		e->internal_uri = requestPath;

	// 1. check if the requested resource is directory
	else if (requestPath.back() == '/')
	{
		std::cout<<"request path is directory"<<std::endl;
		if (requestPath.length() == 1)
			e->internal_uri =  "/";
		else
		{
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
			 * route : root + /abc/
			 * */
			e->internal_uri = tmp.substr(pos);
		}
	}
	// 2. check if the requested resource is file
	else
	{
		std::cout<<"request path is file"<<std::endl;
		if (pos == std::string::npos)
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
			std::cout<<"1"<<std::endl;
			e->internal_uri = "";
		}
		else
		{
			/**
			 * resource identifier exists
			 * */
			/**
			 * path : /test/abc
			 * loc  : /tes
			 * resource : /abc
			 * route : root + /abc
			 * */
			std::cout<<"3"<<std::endl;
			e->internal_uri = tmp.substr(pos);
		}
	}
	std::cout<<"=======!!!!!!!!!!!!!!!!!!!!!!!!!11!!!!=====resource setted============="<<std::endl;
	std::cout<<"internal uri = "<<e->internal_uri<<std::endl;
	std::cout<<"=======!!!!!!!!!!!!!!!!!!!!!!!!!!!1================================"<<std::endl;
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


void EventLoop::ws_internalRedir(Event *e)
{
	setLocationData(e);
	setInternalUri(e);
}

/**
 * error 감지되면 callback하는 함수.
 * 현재 location block상에 error_page가 있으면 그 페이지를 uri로 설정하고,file read event를 등록함.
 * 없으면 바로 404를 write함.
 * status code절대 바뀌면 안됨.
 * */
void EventLoop::errorCallback(Event *e)
{
	// 내부 internal status code로 사용자에게 보여질 status code복사함.
	// 
	e->internal_status = e->getStatusCode();
	/**
	 * reqhandler에서 호출시 없을수도 있음.
	 * */
	if (!e->locationData)
	{
		std::cout<<"handler called... but location data is null"<<std::endl;
		ws_internalRedir(e);
	}
	if (e->setErrorPage())
	{
		/**
		 * error page가 있으면, internal uri, method에 설정된상태임
		 * */
		//internal redirection
		ws_internalRedir(e);
		//errorpage의  internal redirection은 GET으로 간주하고 진행함.
		ws_method_GET(e);
	}
	else
	{
		// 그냥 socket에 404를 쓰고 끝냄.
		unregisterClientSocketReadEvent(e);
		registerClientSocketWriteEvent(e);
	}
}

/**
 * @WARN use INTERNAL URI, METHOD
 * */
void EventLoop::setHttpResponse(Event *e)
{
	HttpreqHandler *reqHandler = static_cast<HttpreqHandler *>(e->getRequestHandler());

	setServerName(e);
	ws_internalRedir(e);
	/**
	 * internal loop
	 * */
	if (!checkAllowedMethods(e))
		errorCallback(e);
	if (!checkClientMaxBodySize(e))
		errorCallback(e);

	/**
	 * client's request is redirection.
	 * */
	if (!e->locationData->getRedirectUrl().empty())
	{
		e->setStatusCode(e->locationData->getReturnStatus());
		unregisterClientSocketReadEvent(e);
		registerClientSocketWriteEvent(e);
		return;
	}
	/**
	 * 7. if need cgi process
	 * */
	if (!e->locationData->getCgiPass().empty())
	{
		std::cout<<"processing cgi..."<<std::endl;
		/**
		 * process cgi
		 * */
		//process cgi
		if (!processCgi(e))
		{
			/**
			 * cgi process error
			 * */
			errorCallback(e);
		}
		return ;
	}
	/**
	 * 8. process methods
	 * */
	int methodIndex = MethodFactory::getInstance().getMethodIndex(e->internal_method);
	/**
	 * if method == GET
	 * */
	if (methodIndex == GET || methodIndex == HEAD)
		ws_method_GET(e);
	else if (methodIndex == POST)
		ws_method_POST(e);
	else if (methodIndex == DELETE)
		ws_method_DELETE(e);
}
