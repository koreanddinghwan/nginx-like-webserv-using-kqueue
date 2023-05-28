#include "commonHttpInfo.hpp"
#include "iostream"
#include "sstream"
#include "HttpResponseInfo.hpp"
#include "responseHandler.hpp"
#include "../eventLoop/Event.hpp"

int main(void)
{

	http_status_msg msg = http_status_msg(404);
	Response res(404);
	//Event *event = new Event(HTTP_SERVER);
	responseHandler *Res = new responseHandler();

	Res->handle(NULL);
	

	std::cout << "===========" << http_status_codes(404) << "==============" << std::endl;
	std::cout << "===========" << msg.getStatusMsg() << "==============" << std::endl;
	std::cout << "===========" << res.getStatusCode() << "==============" << std::endl;
	std::cout << "===========" << res.getStatusMsg() << "==============" << std::endl;

}