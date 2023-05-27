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

	std::vector<IHttpBlock *> serverBlock = static_cast<HttpData *>(this->getConfigData())->getServerBlock();

	for (int i = 0; i < serverBlock.size(); i++)
	{
		/**
		 * @set set ServerNamesByPort
		 * */
		HttpServerData *serverData = static_cast<HttpServerData *>(serverBlock[i]->getConfigData());
		int port = serverData->getListen();

		if (this->serverNamesByPort.find(port) == this->serverNamesByPort.end())
		{
			std::vector<std::string> *serverNames = new std::vector<std::string>();
			for (int i = 0; i < serverData->getServerNames().size(); i++)
				serverNames->push_back(serverData->getServerNames()[i]);
			this->serverNamesByPort[port] = serverNames;
		}
		else
		{
			for (int i = 0; i < serverData->getServerNames().size(); i++)
				this->serverNamesByPort[port]->push_back(serverData->getServerNames()[i]);
		}

		/**
		 * @set locationDatasByPort
		 * */
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
	 * @set defaultServerData
	 * Event 구조체가 들고있어야하는 default server block을 세팅한다.  
	 * 파싱이 완료된 상태이므로, 접근해서 세팅만하면된다.  
	 * */
	this->defaultServerData = &(static_cast<HttpServerBlock *>(this->confData.getServerBlock()[0])->getServerData());
}

HttpBlock::~HttpBlock() {
	for (int i = 0; i < this->confData.getServerBlock().size(); i++)
		delete  static_cast<HttpServerBlock *>(this->confData.getServerBlock()[i]);
	for (serverNamesByPortMapIter it = this->serverNamesByPort.begin(); it != this->serverNamesByPort.end(); it++)
		delete it->second;
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

std::map<int, std::vector<HttpLocationData *> *>& HttpBlock::getLocationDatasByPort()
{
	return (this->locationDatasByPort);
}

std::vector<HttpLocationData *> *HttpBlock::findLocationDatasByPort(int p)
{
	return this->locationDatasByPort.find(p)->second;
}

HttpBlock::serverNamesByPortMapRef HttpBlock::getServerNamesByPort()
{
	return this->serverNamesByPort;
}

HttpBlock::HttpBlock() {}
IConfigData* HttpBlock::getConfigData() {return &confData;}
HttpData& HttpBlock::getHttpData() {return confData;}
HttpServerData *HttpBlock::getDefaultServerData() {return this->defaultServerData;}
