#include "HttpBlock.conf.hpp"
#include "HttpLocationBlock.conf.hpp"
#include <utility>

HttpBlock::HttpBlock(std::ifstream &File) 
{
	this->parse(File);
	// parsing part is done.

	/**
	 * private 멤버변수 세팅
	 * */

	std::vector<HttpServerBlock *> serverBlock = static_cast<HttpData *>(this->getConfigData())->getServerBlock();

	for (size_t i = 0; i < serverBlock.size(); i++)
	{
		int serverPort = serverBlock.at(i)->getServerData().getListen();
		int flag = 0;

		for (size_t i = 0; i < this->identicalPorts.size(); i++)
		{
			if (this->identicalPorts[i] == serverPort)
			{
				flag = 1;
				break;
			}
		}
		if (!flag)
			this->identicalPorts.push_back(serverPort);
	}
}

HttpBlock::~HttpBlock() 
{
	for (size_t i = 0; i < this->confData.getServerBlock().size(); i++)
		delete  static_cast<HttpServerBlock *>(this->confData.getServerBlock()[i]);
}


void HttpBlock::parse(std::ifstream &File) 
{
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

		BlockParser::httpBlockParser(buf, *static_cast<HttpConfigData *>(this->getConfigData()));

		if (buf.find("server {") != std::string::npos)
		{
			this->getHttpData().setServerBlock(
					new HttpServerBlock(File, (&this->getHttpData()))
				);
		}
	}
}

HttpBlock::HttpBlock() {}
IConfigData* HttpBlock::getConfigData() {return &confData;}
HttpData& HttpBlock::getHttpData() {return confData;}

std::vector<int>& HttpBlock::getIdenticalPorts()
{return this->identicalPorts;}
