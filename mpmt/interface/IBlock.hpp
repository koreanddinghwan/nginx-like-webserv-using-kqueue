#ifndef IBLOCK_HPP
# define IBLOCK_HPP

#include "IConfigData.hpp"
#include <fstream>

class IBlock
{
public:
	virtual void parse(std::ifstream &File) = 0;
	virtual IConfigData* getConfigData() = 0;
};

#endif
