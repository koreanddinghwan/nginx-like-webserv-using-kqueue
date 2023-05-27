#include "response.hpp"
#include "HttpreqHandler.hpp"

void *Response::handle(void *data)
{
	std::cout<<"\033[36m"<<"@@@@@@@@@@@@@@@@Response handle@@@@@@@@@@@@@@@@@"<<std::endl;
	Event *e = static_cast<Event *>(data);


	static_cast<HttpreqHandler *>(e->getRequestHandler())->printReq();

	for (int i = 0; i < (e->getLocationData())->size(); i++)
	{
		(*e->getLocationData())[i]->printLocationData();
	}

	if (!checkServerNameExistsInConfig(e))
	{
		/**
		 * server name이 없다면, default server사용
		 * */
	}
	/**
	 * find location data by longest prefix match in event's locationData
	 * */
	std::cout << "use longest prefix match" << std::endl;


	return NULL;
}

/**
 * @1 check host name and server name
 * client request의 hostname과 server name을 비교한다.
 * 다르다면, default server
 * https://nginx.org/en/docs/http/server_names.html#optimization
 * port번호로 바인딩된 해시맵에서 확인
 * */
bool Response::checkServerNameExistsInConfig(Event *e)
{
	std::vector<std::string> *serverNames = Config::getInstance().getHTTPBlock()->getServerNamesByPort()[(*e->getLocationData())[0]->getListen()];

	std::string host = static_cast<HttpreqHandler *>(e->getRequestHandler())->getRequestInfo().reqHeaderMap.find("Host")->second;
	for (int i = 0; i < serverNames->size(); i++)
	{
		if (host == serverNames->at(i))
			return true;
	}
	return false;
}

bool Response::checkAllowMethod()
{
	return true;
}


Response::Response()
{
}

Response::~Response()
{
}
