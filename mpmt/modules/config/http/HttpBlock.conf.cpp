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

	std::vector<HttpServerBlock *> serverBlock = static_cast<HttpData *>(this->getConfigData())->getServerBlock();

	for (int i = 0; i < serverBlock.size(); i++)
	{
		/**
		 * @set locationDatasByPort
		 * */
		std::vector<HttpLocationBlock *> locationBlock = static_cast<HttpServerData *>(serverBlock[i]->getConfigData())->getHttpLocationBlock();

		for (int j = 0; j < locationBlock.size(); j++)
		{
			int port = locationBlock[j]->getLocationData().getListen();

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

	/*/1** */
	/* * @set defaultServerData */
	/* * Event 구조체가 들고있어야하는 default server block을 세팅한다. */  
	/* * 파싱이 완료된 상태이므로, 접근해서 세팅만하면된다. */  
	/* * *1/ */
	/*this->defaultServerData = &(static_cast<HttpServerBlock *>(this->confData.getServerBlock()[0])->getServerData()); */
}

HttpBlock::~HttpBlock() 
{
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
			this->getHttpData().setServerBlock(
					new HttpServerBlock(File, (&this->getHttpData()))
				);
		}
	}
}

std::map<int, std::vector<HttpLocationData *> *>& HttpBlock::getLocationDatasByPort()
{
	return (this->locationDatasByPort);
}


HttpBlock::HttpBlock() {}
IConfigData* HttpBlock::getConfigData() {return &confData;}
HttpData& HttpBlock::getHttpData() {return confData;}
