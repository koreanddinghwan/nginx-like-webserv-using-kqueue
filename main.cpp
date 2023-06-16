#include <exception>
#include <iostream>
#include "./modules/webserv/Webserv.hpp"
#include "./modules/webserv/WebservManager.hpp"

int main(int ac, char **av) 
{
	try {
		WebServ::getInstance().run(ac, av);
	} catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
	}
	return 0;
}
