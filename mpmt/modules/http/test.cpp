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
}