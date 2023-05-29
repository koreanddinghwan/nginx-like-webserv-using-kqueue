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
	std::ifstream test ("index.html", std::ios_base::in);
	std::string s;
	responseHandler *Res = new responseHandler(400);
	std::cout << "========" << Res->getResStatusCode() << std::endl;
	std::cout << "========" << Res->getResStatusMsg() << std::endl;

	if (test.is_open()) {
		test.seekg(0, std::ios::end);
		int size = test.tellg();
		s.resize(size);
		test.seekg(0, std::ios::end);
		test.read(&s[0], size);
	}
	int i = 0;
	std::cout<< s << std::endl;
	while (i < 3) {
		Res->setResBody(s);
		Res->setResBody("======================================\n");
		i++;
	}
	std::cout << "===============" << Res->getResBody().capacity() << "================\n" <<std::endl;
	std::cout << "===============" << Res->getResBody().size() << "================\n" <<std::endl;
	Res->setResStatusCode(301);
	Res->setResStatusMsg(301);

	//Event *event = new Event(HTTP_SERVER);

	Res->handle(NULL);
}