#ifndef IBLOCK_HPP
# define IBLOCK_HPP

#include <fstream>
class IBlock
{
public:
	virtual void parse(std::ifstream &File) = 0;
	virtual void *getConfigData() = 0;
};

#endif
