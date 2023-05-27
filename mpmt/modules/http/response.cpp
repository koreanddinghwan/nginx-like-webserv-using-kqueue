#include "response.hpp"

void *Response::handle(void *data)
{
	std::cout<<"\033[36m"<<"@@@@@@@@@@@@@@@@Response handle@@@@@@@@@@@@@@@@@"<<std::endl;
	Event *event = static_cast<Event *>(data);


	static_cast<HttpreqHandler *>(event->getRequestHandler())->printReq();

	for (int i = 0; i < (event->getLocationData())->size(); i++)
	{
		(*event->getLocationData())[i]->printLocationData();
	}

	/**
	 * @1 check host name and server name
	 * client request의 hostname과 server name을 비교한다.
	 * 다르다면, default server
	 * https://nginx.org/en/docs/http/server_names.html#optimization
	 * port번호로 바인딩된 해시맵에서 확인
	 * */

	return NULL;
}

bool Response::checkServerName(Event::t_locationData *locationData)
{
	for (int i = 0; i < locationData->size(); i++)
	{
		(*locationData)[i]->getServerNames();
	}
	return true;
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
