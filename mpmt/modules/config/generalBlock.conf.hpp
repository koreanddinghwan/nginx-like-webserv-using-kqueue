#ifndef GENERALBLOCK_HPP
# define GENERALBLOCK_HPP

#include <cstdlib>
#include "../../interface/IBlock.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include "../../lib/strSplit.hpp"
#include <cstdlib>

class generalBlock: public IBlock
{
public:
	struct generalConfig {
		int worker_processes;
	};

private:
	generalConfig confData;

public:
	generalBlock(std::ifstream &File) {
		this->parse(File);
	}

	void parse(std::ifstream &File) 
	{
		std::string buf;

		std::getline(File, buf);
		if (buf.find("worker_processes") == std::string::npos)
		{
			File.seekg(0);
			this->confData.worker_processes = 0;
			std::cout<<"\033[31m"<<"worker_processes is set 0"<< "\033[0m" <<std::endl;
			return ;
		}
		else
		{
			strSplit spl;

			strSplit::strPair splited = spl.split(buf, ' ');
			this->confData.worker_processes = std::atoi(splited.second.c_str());
		}
	}
		/**
		 * @brief get generalConfig data
		 *
		 *
		 * @return generalConfigData
		 * struct generalConfig {
		 * int worker_process;
		 * };
		 */
	void *getConfigData() {
		return &this->confData;
	}

	~generalBlock() {}
};

#endif
