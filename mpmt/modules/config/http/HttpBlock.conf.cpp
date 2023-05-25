#include "httpBlock.conf.hpp"
#include "HttpLocationBlock.conf.hpp"
#include <utility>

HttpBlock::HttpBlock(std::ifstream &File) 
{
	this->parse(File);
	// parsing part is done.

	/**
	 * private 멤버변수 세팅
	 * */

	/**
	 * @set locationDatasByPort
	 * locationDatasByPort는 port를 key로, 해당 port를 가진 location block들을 value로 가지는 map이다.
	 * */
	std::vector<IHttpBlock *> serverBlock = static_cast<HttpData *>(this->getConfigData())->getServerBlock();

	for (int i = 0; i < serverBlock.size(); i++)
	{
		std::vector<IHttpBlock *> locationBlock = static_cast<HttpServerData *>(serverBlock[i]->getConfigData())->getHttpLocationBlock();

		for (int j = 0; j < locationBlock.size(); j++)
		{
			int port = static_cast<HttpLocationData *>(locationBlock[j]->getConfigData())->getListen();

			/**
			 * 해당 port를 가진 location block이 없음. 
			 * */
			if (locationDatasByPort.find(port) == locationDatasByPort.end())
			{
				/**
				 * 동적할당, 소멸자에서 해제
				 * */
				std::vector<HttpLocationData *> *tmp = new std::vector<HttpLocationData *>();
				tmp->push_back(static_cast<HttpLocationData *>(&static_cast<HttpLocationBlock *>(locationBlock[j])->getLocationData()));
				locationDatasByPort[port] = tmp;
			}
			/**
			 * 해당 port를 가진 location block이 있음.
			 * */
			else
				locationDatasByPort.find(port)->second->push_back(static_cast<HttpLocationData *>(&static_cast<HttpLocationBlock *>(locationBlock[j])->getLocationData()));
		}
	}

	/**
	 * @set serverdatas
	 * Event 구조체가 들고있어야하는 server datas를 세팅한다.
	 * 파싱이 완료된 상태이므로, 접근해서 세팅만하면된다.
	 * */
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
