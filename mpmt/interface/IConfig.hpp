#ifndef ICONFIG_HPP
# define ICONFIG_HPP

#include "IServer.hpp"
#include <vector>

class IConfig
{
public:

	virtual std::vector<IServer *> *getServers() = 0;

	~IConfig() {}
};

#endif
