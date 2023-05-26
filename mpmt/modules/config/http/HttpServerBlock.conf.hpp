#ifndef HTTPSERVERBLOCK_HPP
# define HTTPSERVERBLOCK_HPP

#include "HttpLocationBlock.conf.hpp"
#include "../BlockParser.hpp"
#include "../data/HttpServerData.hpp"
#include "../../../lib/ft_split.hpp"
#include "../../../lib/strSplit.hpp"
#include "../../../interface/IHttpBlock.hpp"
#include <fstream>
#include <vector>
#include <string>
#include <iostream>

class HttpData;

class HttpServerBlock : public IHttpBlock
{
private:
	HttpServerData serverData;

public:
	HttpServerBlock(std::ifstream &File, HttpData *c);
	IConfigData* getConfigData();
	HttpServerData &getServerData();
	~HttpServerBlock();

private:
	void parse(std::ifstream &File);
};

#endif
