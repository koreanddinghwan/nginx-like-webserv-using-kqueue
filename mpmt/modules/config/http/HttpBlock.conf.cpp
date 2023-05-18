#include "httpBlock.conf.hpp"
#include "HttpLocationBlock.conf.hpp"
#include <utility>

HttpBlock::HttpBlock(std::ifstream &File) 
{
	this->parse(File);
	std::vector<IHttpBlock *> serverBlock = static_cast<HttpData *>(this->getConfigData())->getServerBlock();

	for (int i = 0; i < serverBlock.size(); i++)
	{
		std::vector<IHttpBlock *> locationBlock = static_cast<HttpServerData *>(serverBlock[i]->getConfigData())->getHttpLocationBlock();

		for (int j = 0; j < locationBlock.size(); j++)
		{
			int port = static_cast<HttpLocationData *>(locationBlock[j]->getConfigData())->getListen();

			/* 해당 port를 가진 location block이 없ㅇ므. */
			if (locationDatasByPort.find(port) == locationDatasByPort.end())
			{
				std::vector<HttpLocationData *> *tmp = new std::vector<HttpLocationData *>();
				tmp->push_back(static_cast<HttpLocationData *>(&static_cast<HttpLocationBlock *>(locationBlock[j])->getLocationData()));
				locationDatasByPort[port] = tmp;
			}
			else
				locationDatasByPort.find(port)->second->push_back(static_cast<HttpLocationData *>(&static_cast<HttpLocationBlock *>(locationBlock[j])->getLocationData()));
		}
	}
}

HttpBlock::~HttpBlock() {
	for (int i = 0; i < this->confData.getServerBlock().size(); i++)
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
			std::cout<<"\033[32m"<<"make new server block:" <<buf<<std::endl;
			static_cast<HttpData *>(this->getConfigData())->setServerBlock(new HttpServerBlock(File, static_cast<HttpData *>(this->getConfigData())));
		}
	}
}

std::vector<HttpLocationData *> *HttpBlock::findLocationDatasByPort(int p)
{
	return this->locationDatasByPort.find(p)->second;
}

std::map<int, std::vector<HttpLocationData *> *>& HttpBlock::getLocationDatasByPort()
{
	return (this->locationDatasByPort);
}

HttpBlock::HttpBlock() {}
IConfigData* HttpBlock::getConfigData() {return &confData;}
