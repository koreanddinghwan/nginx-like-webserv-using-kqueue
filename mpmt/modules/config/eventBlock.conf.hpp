#ifndef EVENTBLOCK_HPP
# define EVENTBLOCK_HPP

#include "../../interface/IBlock.hpp"
#include <fstream>

class eventBlock: public IBlock
{
public:
	eventBlock(std::ifstream &File) {
		this->parse();
	}

	void parse() {}



	~eventBlock() {}
};

#endif
