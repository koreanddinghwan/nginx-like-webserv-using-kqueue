#include <iostream>
#include "./modules/config/SingletonConfig.hpp"
#include "./modules/manager/WebservManager.hpp"
#include "webserv.hpp"

int main(int ac, char **av) 
{
	WebServ::getInstance().init(ac, av);
	WebServ::getInstance().run();
	return 0;
}
