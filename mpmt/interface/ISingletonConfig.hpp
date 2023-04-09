#ifndef ISINGLETONCONFIG_HPP
# define ISINGLETONCONFIG_HPP

#include "./IBlock.hpp"

class ISingletonConfig
{
public:
	virtual IBlock **getBlocks() = 0;
};

#endif
