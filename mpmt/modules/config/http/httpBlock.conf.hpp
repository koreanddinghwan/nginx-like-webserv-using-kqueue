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
	struct httpData 
	{
		int client_max_body_size;
		bool sendfile;
		bool tcp_nopush;
		std::vector<IBlock *> httpServerBlock;
	};

private:
	httpData confData;

public:
	HttpBlock(std::ifstream &File);
	~HttpBlock();


private:
	void parse(std::ifstream &File);
	void *getConfigData();
};

#endif
