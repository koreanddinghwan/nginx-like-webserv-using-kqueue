#ifndef IBLOCK_HPP
# define IBLOCK_HPP

#include "../modules/config/data/ConfigData.hpp"
#include <fstream>
class IBlock
{
public:
	virtual void parse(std::ifstream &File) = 0;
	virtual ConfigData& getConfigData() = 0;
};

#endif
