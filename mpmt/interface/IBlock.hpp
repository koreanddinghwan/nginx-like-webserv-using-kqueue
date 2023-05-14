#ifndef IBLOCK_HPP
# define IBLOCK_HPP

#include "../modules/config/ConfigData.hpp"
#include <fstream>
class IBlock
{
public:
	virtual void parse(std::ifstream &File) = 0;
	virtual ConfigData& getConfigData() = 0;
};

#endif
