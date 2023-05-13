#include "./HttpBlock.conf.hpp"
#include "HttpServerBlock.conf.hpp"
#include <string>
#include <vector>

HttpBlock::HttpBlock(std::ifstream &File) 
{
		this->parse(File);
}

HttpBlock::~HttpBlock() {
	for (std::vector<IBlock *>::iterator it = this->confData.httpServerBlock.begin(); it != this->confData.httpServerBlock.end(); it++)
	{
		delete static_cast<HttpServerBlock *>(*it);
	}
}


void HttpBlock::parse(std::ifstream &File) 
{
	int cur_offset = File.tellg();

	std::string buf;

	std::getline(File, buf);

	//http 찾을때까지 점프
	while (buf.find("http") == std::string::npos)
		std::getline(File, buf);

	//http 찾으면 } 찾을때까지 점프
	while (buf.find("}") == std::string::npos)
	{
		//server block찾을때까지 점프
		while (buf.find("server") == std::string::npos)
			std::getline(File, buf);
		
		this->confData.httpServerBlock.push_back(new HttpServerBlock(File));
		//server block찾으면 } 찾을때까지 점프
	}


	/*
	 * while 'server' directives in buf,
	 * {
	 * 		httpServerBlock.push(new httpServerBlock(File));
	 * }
	 * */
}

HttpBlock::httpData& HttpBlock::getConfigData()
{
	return (this->confData);
}
