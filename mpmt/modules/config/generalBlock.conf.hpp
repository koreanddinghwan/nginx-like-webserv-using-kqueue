#ifndef GENERALBLOCK_HPP
# define GENERALBLOCK_HPP

#include "../../interface/IBlock.hpp"
#include <fstream>

class generalBlock: public IBlock
{
public:
	generalBlock(std::ifstream &File) {
		this->parse();
	}

	void parse() {}



	~generalBlock() {}
};

#endif
