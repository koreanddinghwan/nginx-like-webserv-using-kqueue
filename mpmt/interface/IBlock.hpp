#ifndef IBLOCK_HPP
# define IBLOCK_HPP

#include "../modules/config/data/HttpConfigData.hpp"
#include <fstream>

class IBlock
{
public:
	virtual void parse(std::ifstream &File) = 0;
	virtual HttpConfigData& getConfigData() = 0;
};

#endif
