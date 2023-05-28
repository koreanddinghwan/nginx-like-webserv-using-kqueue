#include <exception>
#include <iostream>
#include "./modules/webserv/Webserv.hpp"
#include "./modules/webserv/WebservManager.hpp"

int main(int ac, char **av) 
{
	WebServ::getInstance().run(ac, av);
	return 0;
}
