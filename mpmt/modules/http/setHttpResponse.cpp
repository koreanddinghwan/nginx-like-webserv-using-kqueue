#include "setHttpResponse.hpp"

int getLongestPrefixMatchScore(const std::string& location, const std::string& requestPath);

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
void setHttpResponse(Event *e)
{
	HttpreqHandler *reqHandler = static_cast<HttpreqHandler *>(e->getRequestHandler());


	/**
	 * 1. check host name in server names
	 * */
	// 효율 안좋다 ..
	std::string host = reqHandler->getRequestInfo().reqHeaderMap.find("Host")->second;
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
				break ;
			}
		}
	}

	/**
	 * 2. check the host's uri in location datas by prefix matching
	 * */
	std::string requestPath = reqHandler->getRequestInfo().path;
	HttpLocationData *locationData = NULL;
	int matchScore = 0;
	for (int i = 0; i < e->getDefaultServerData()->getLocationDatas().size(); i++)
	{
		std::string &locationUri = e->getDefaultServerData()->getLocationDatas().at(i)->getUri();
		if (matchScore < getLongestPrefixMatchScore(locationUri, requestPath))
		{
			matchScore = getLongestPrefixMatchScore(locationUri, requestPath);
			locationData = e->getDefaultServerData()->getLocationDatas().at(i);
		}
	}
	if (!locationData)
	{
		e->setStatusCode(404);
		throw std::exception();
	}

	/**
	 * 3. check the allowed_methods in location data
	 * */
	int methodIndex = MethodFactory::getInstance().getMethodIndex(reqHandler->getRequestInfo().method);
	if (!(locationData->getLimitedMethods().methods[methodIndex]))
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

	/**
	 * 4. make resource route in server with root route
	 * */
	e->setRoute(locationData->getRoot() 
			+ "/" 
			+ reqHandler->getRequestInfo().path.substr(
				locationData->getUri().length()
				)
			);


	/**
	 * 5. if redirecturl exists, redirect to url
	 * */
	if (!locationData->getRedirectUrl().empty())
	{
		e->setStatusCode(301);
		throw std::exception();
	}

	if (!locationData->getCgiPass().empty())
	{
		/**
		 * process cgi
		 * */
		return ;
	}

	/**
	 * if method == GET
	 * */
	if (methodIndex == GET)
	{
		/**
		 * check if the resource exists
		 * */
	}
	
	if (methodIndex == POST)
	{}

	if (methodIndex == DELETE)
	{}
}

int getLongestPrefixMatchScore(const std::string& location, const std::string& requestPath) {
    // Find the length of the shortest string
    size_t length = std::min(location.length(), requestPath.length());

    // Find the index where the location and requestPath differ
    size_t index = 0;
    while (index < length && location[index] == requestPath[index]) {
        index++;
    }
    // Return the matched prefix length
    return location.substr(0, index).length();
}
