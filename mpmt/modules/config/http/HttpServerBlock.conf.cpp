#include "HttpServerBlock.conf.hpp"

HttpServerBlock::HttpServerBlock(std::ifstream &File)
{
	this->parse(File);
}

HttpServerBlock::HttpServerData& HttpServerBlock::getConfigData() {return this->serverData;}

HttpServerBlock::~HttpServerBlock() {}

void HttpServerBlock::parse(std::ifstream &File) 
{
	/*
	 * while 'location' directives in buf,
	 * {
	 * 		httpServerBlock.push(new httpLocationBlock(File));
	 * }
	 * */
}
