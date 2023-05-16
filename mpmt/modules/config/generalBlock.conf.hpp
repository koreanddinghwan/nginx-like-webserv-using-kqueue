#ifndef GENERALBLOCK_HPP
# define GENERALBLOCK_HPP

#include <cstdlib>
#include "../../interface/IBlock.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include "../../lib/strSplit.hpp"
#include <cstdlib>

class GeneralBlock: public IBlock
{
public:
	struct generalConfig : public HttpConfigData
	{
		int worker_processes;
	};

private:
	generalConfig confData;

public:
	GeneralBlock(std::ifstream &File);
	void parse(std::ifstream &File);
	generalConfig& getConfigData();
	~GeneralBlock();
};

#endif
