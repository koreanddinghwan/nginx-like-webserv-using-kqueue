#ifndef ISINGLETONCONFIG_HPP
# define ISINGLETONCONFIG_HPP

#include "../exceptions/configParseException.hpp"
#include "./IHttpBlock.hpp"
#include <string>

class IConfig
{
public:
	virtual IBlock *getGeneralBlock() = 0;
	virtual IBlock *getEventBlock() = 0;
	virtual	IHttpBlock *getHTTPBlock() = 0;
	virtual	IBlock *getSMTPBlock() = 0;
	virtual	IBlock *getFTPBlock() = 0;
	virtual	IBlock *getSTREAMBlock() = 0;
	virtual void initConfig(std::string path) throw (configParseException) = 0;
};

#endif
