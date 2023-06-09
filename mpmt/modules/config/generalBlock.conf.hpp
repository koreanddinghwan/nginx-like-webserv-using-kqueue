#ifndef GENERALBLOCK_HPP
# define GENERALBLOCK_HPP

#include <cstdlib>
#include "../../interface/IConfigData.hpp"
#include "../../interface/IGeneralBlock.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include "../../lib/strSplit.hpp"
#include <cstdlib>

class GeneralBlock: public IGeneralBlock
{
public:
	struct generalConfig : public IConfigData
	{
		int worker_processes;
	};

private:
	generalConfig confData;

public:
	GeneralBlock(std::ifstream &File);
	void parse(std::ifstream &File);
	IConfigData* getConfigData();
	~GeneralBlock();
};

#endif
