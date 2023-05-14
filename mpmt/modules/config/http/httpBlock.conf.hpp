#ifndef HTTPBLOCK_HPP
# define HTTPBLOCK_HPP

#include "../../../interface/IBlock.hpp"
#include "HttpServerBlock.conf.hpp"
#include <fstream>
#include <iostream>
#include <vector>

/**
 * @brief make Http block
 * Http block 1... <-> ...n server block 1... <-> ...n location block
 */
class HttpBlock: public IBlock
{
public:
	class httpData: public ConfigData
	{
		private:
			std::vector<IBlock *> httpServerBlock;

		public:
			httpData();
			~httpData();

			std::vector<IBlock *> getServerBlock();
			void setServerBlock(HttpServerBlock *f);
	};


private:
	httpData confData;

public:
	HttpBlock(std::ifstream &File);
	~HttpBlock();


private:
	void parse(std::ifstream &File);
	httpData &getConfigData();
};

#endif
