#include "modules/config/Config.hpp"
#include "modules/server/Server.hpp"
#include "modules/socket/Socket.hpp"


int main(int ac, char **av) {
	
	Config config(av[1]);

	config.configuration();
	Socket socketModule(&config);	


	return 0;
}
