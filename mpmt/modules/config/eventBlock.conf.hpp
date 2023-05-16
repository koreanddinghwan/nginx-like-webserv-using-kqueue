#ifndef EVENTBLOCK_HPP
# define EVENTBLOCK_HPP

#include "../../lib/strSplit.hpp"
#include "../../interface/IBlock.hpp"
#include <fstream>
#include <iostream>
#include <string>

class EventBlock: public IBlock
{
public:
	struct eventConfig : public HttpConfigData
	{
		int worker_connections;
	};

private:
	eventConfig confData;

public:
	EventBlock(std::ifstream &File);
	void parse(std::ifstream &File);
	eventConfig& getConfigData();
	~EventBlock();
};

#endif
