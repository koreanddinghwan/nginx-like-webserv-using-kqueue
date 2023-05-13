#include "HttpBlock.conf.hpp"

HttpBlock::HttpBlock(std::ifstream &File) 
{
		this->parse(File);
}
HttpBlock::~HttpBlock() {}


void HttpBlock::parse(std::ifstream &File) 
{
	int cur_offset = File.tellg();

	std::string buf;

	std::getline(File, buf);

	std::cout<<buf<<std::endl;
	/*
	 * while 'server' directives in buf,
	 * {
	 * 		httpServerBlock.push(new httpServerBlock(File));
	 * }
	 * */
}

void *HttpBlock::getConfigData()
{
	return &(this->confData);
}
