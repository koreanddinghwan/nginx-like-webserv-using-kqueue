#ifndef HTTPBLOCK_HPP
# define HTTPBLOCK_HPP

#include "../data/HttpData.hpp"
#include "../../../interface/IHttpBlock.hpp"
#include "HttpLocationBlock.conf.hpp"
#include "HttpServerBlock.conf.hpp"
#include "../BlockParser.hpp"
#include <fstream>
#include <iostream>
#include <vector>
#include <set>

/**
 * @brief make Http block
 * Http block 1... <-> ...n server block 1... <-> ...n location block
 */
class HttpBlock: public IHttpBlock
{
private:
	HttpData confData;
	std::vector<int> identicalPorts;

public:
	HttpBlock(std::ifstream &File);

	IConfigData* getConfigData();
	HttpData& getHttpData();
	std::vector<int>& getIdenticalPorts();
	~HttpBlock();
	
private:
	/**
	 * Do not enable default constructor
	 * */
	HttpBlock();

	/**
	 * private method for parsing
	 * */
	void parse(std::ifstream &File);
};

#endif
