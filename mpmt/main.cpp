#include <iostream>
#include "./modules/config/SingletonConfig.hpp"
#include "./modules/manager/WebservManager.hpp"
#include "webserv.hpp"

int main(int ac, char **av) 
{

	if (ac > 1)
	{
		std::cerr<<"Please input 1 config file or nothing"<<std::endl;
		exit(1);
	}

	//config 모듈 초기화
	SingletonConfig::getInstance().initSingletonConfig(av[1]);


	WebServ::getInstance().run();

	/* webserv.run(); */
	return 0;
}
