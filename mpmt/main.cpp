#include <exception>
#include <iostream>
#include "./modules/manager/WebservManager.hpp"
#include "webserv.hpp"

int main(int ac, char **av) 
{
	try {
		WebServ::getInstance().init(ac, av);
	} catch (std::exception &e) {
		std::cout<<"WebServ initializing failed"<<std::endl;
		std::cout<<e.what()<<std::endl;
		return 0;
	}
	WebServ::getInstance().run();
	return 0;
}
