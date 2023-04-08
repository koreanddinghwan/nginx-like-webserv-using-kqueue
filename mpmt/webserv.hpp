#ifndef WEBSERV_HPP
# define WEBSERV_HPP

#include "interface/IBlock.hpp"
#include "interface/IServer.hpp"
#include "modules/config/Config.hpp"
#include <stdexcept>
#include <vector>

class WebServ
{
private:
	Config *config;
	IBlock **blocks;
	IServer **servers;


public:
	WebServ(Config *conf) : config(conf)
	{
		this->blocks = config->getBlocks();
	}

	~WebServ() {}

	void run() throw (std::runtime_error) {
		
	}
};

#endif
