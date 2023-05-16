#ifndef EVENTBLOCK_HPP
# define EVENTBLOCK_HPP

#include "../../lib/strSplit.hpp"
#include "../../interface/IEventBlock.hpp"
#include <fstream>
#include <iostream>
#include <string>

class EventBlock: public IEventBlock
{
public:
	class eventConfig: public IConfigData
	{
		public:
			int worker_connections;
	};

private:
	eventConfig confData;

public:
	EventBlock(std::ifstream &File);
	void parse(std::ifstream &File);
	IConfigData *getConfigData();
	~EventBlock();
};

#endif
