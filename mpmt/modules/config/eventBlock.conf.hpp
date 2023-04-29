#ifndef EVENTBLOCK_HPP
# define EVENTBLOCK_HPP

#include "../../lib/strSplit.hpp"
#include "../../interface/IBlock.hpp"
#include <fstream>
#include <iostream>
#include <string>

class eventBlock: public IBlock
{
public:
	struct eventConfig {
		int worker_connections;
	};

private:
	eventConfig confData;

public:
	eventBlock(std::ifstream &File) {
		this->parse(File);
	}

	void parse(std::ifstream &File) 
	{
		int cur_offset = File.tellg();
		std::string buf;

		std::getline(File, buf);
		while (buf.length() == 0)
			std::getline(File, buf);

		std::cout<<buf<<std::endl;
		if (buf.find("events") == std::string::npos)
		{
			File.seekg(cur_offset);
			this->confData.worker_connections = 1024;
			std::cout<<"\033[31m"<<"worker_connection is set 1024"<< "\033[0m" <<std::endl;
			return ;
		}
		else
		{
			strSplit spl;

			std::getline(File, buf);
			strSplit::strPair splited = spl.split(buf, ' ');
			this->confData.worker_connections = std::atoi(splited.second.c_str());

			//push into next field
			while (buf.find("}") == std::string::npos)
				std::getline(File, buf);
		}
	}


	void *getConfigData() {
		return &this->confData;
	}

	~eventBlock() {}
};

#endif
