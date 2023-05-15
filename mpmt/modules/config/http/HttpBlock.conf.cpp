#include "HttpBlock.conf.hpp"
#include "HttpServerBlock.conf.hpp"
#include <string>
#include "../Parser.cpp"
#include <vector>

HttpBlock::HttpBlock(std::ifstream &File) 
{
		this->parse(File);
}

HttpBlock::~HttpBlock() {
	for (int i = 0; i < this->confData.getServerBlock().size(); i++)
		delete  static_cast<HttpServerBlock *>(this->confData.getServerBlock()[i]);
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
		if (File.eof())
			break;

		std::getline(File, buf);

		Parser::httpBlockParser(buf, this->getConfigData());

		if (buf.find("server {") != std::string::npos)
		{
			std::cout<<"\033[32m"<<"make new server block:" <<buf<<std::endl;
			this->getConfigData().setServerBlock(new HttpServerBlock(File, this->getConfigData()));
		}
	}
}

HttpBlock::httpData& HttpBlock::getConfigData()
{
	return (this->confData);
}

/*
 * http block 전용 data getter setter
 * */
HttpBlock::httpData::httpData() {}

HttpBlock::httpData::~httpData() {}
std::vector<IBlock *> HttpBlock::httpData::getServerBlock() {return this->httpServerBlock;}


void HttpBlock::httpData::setServerBlock(HttpServerBlock *serverBlock)
{
	this->httpServerBlock.push_back(serverBlock);
}
