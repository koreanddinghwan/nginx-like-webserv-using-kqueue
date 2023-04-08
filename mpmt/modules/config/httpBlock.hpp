#ifndef HTTPBLOCK_HPP
# define HTTPBLOCK_HPP

#include "../../interface/IBlock.hpp"
#include <fstream>

class httpBlock: public IBlock
{
public:
	httpBlock(std::ifstream &File) {
		this->parse();
	}

	void parse() {}



	~httpBlock() {}

};

#endif
