#include "httpBlock.conf.hpp"
#include "HttpLocationBlock.conf.hpp"

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
			if (locationBlocksByPort.find(port) == locationBlocksByPort.end())
				locationBlocksByPort.insert(std::make_pair(port, locationBlock));
			else
				locationBlocksByPort.find(port)->second.push_back(locationBlock[j]);
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

std::vector<HttpLocationBlock *> *HttpBlock::getLocationBlocksByPort(int p)
{
	return &this->locationBlocksByPort.find(p)->second;
}

IConfigData* HttpBlock::getConfigData() {return &confData;}
