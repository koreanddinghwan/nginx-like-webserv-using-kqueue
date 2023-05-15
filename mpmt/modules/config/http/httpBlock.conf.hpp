#ifndef HTTPBLOCK_HPP
# define HTTPBLOCK_HPP

#include "../data/HttpData.hpp"
#include "../../../interface/IBlock.hpp"
#include "HttpServerBlock.conf.hpp"
#include "../BlockParser.hpp"
#include <fstream>
#include <iostream>
#include <vector>

/**
 * @brief make Http block
 * Http block 1... <-> ...n server block 1... <-> ...n location block
 */
class HttpBlock: public IBlock
{
private:
	HttpData confData;

public:
	HttpBlock(std::ifstream &File);
	~HttpBlock();


private:
	void parse(std::ifstream &File);
	HttpData &getConfigData();
};

#endif
