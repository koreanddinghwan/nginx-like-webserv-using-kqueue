#ifndef IMANAGER_HPP
# define IMANAGER_HPP

#include "IModule.hpp"
#include <stdexcept>

class IManager
{
public:
	virtual void initModules(IModule *modules) throw (std::runtime_error) = 0;
	virtual void runManager() throw (std::runtime_error) = 0;
	virtual ~IManager() {}
};

#endif
