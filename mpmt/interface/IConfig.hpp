#ifndef ISINGLETONCONFIG_HPP
# define ISINGLETONCONFIG_HPP

#include "../exceptions/configParseException.hpp"
#include "./IBlock.hpp"
#include <string>

class IConfig
{
public:
	virtual IBlock *getGeneralBlock() = 0;
	virtual IBlock *getEventBlock() = 0;
	virtual	IBlock *getHttpBlock() = 0;
	virtual void initConfig(std::string path) throw (configParseException) = 0;
};

#endif
