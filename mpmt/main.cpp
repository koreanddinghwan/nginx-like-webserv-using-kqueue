#include "modules/config/Config.hpp"
#include "webserv.hpp"


int main(int ac, char **av) {

	if (ac > 1)
	{
		std::cerr<<"Please input 1 config file or nothing"<<std::endl;
		exit(1);
	}

	Config config(av[1]);
	WebServ webserv(&config);
	webserv.run();
	return 0;
}
