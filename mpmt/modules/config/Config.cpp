#include "Config.hpp"

Config& Config::getInstance() {
	static Config instance;
	return instance;
}

/**
 * @brief config file 파싱해서 block별로 정리
 *
 * @param path
 */
void Config::initConfig(std::string path) throw (configParseException)
{
	/*
	 * 파일을 열어두고, 참조자로 전달하면서 파싱 컨텍스트를 유지합니다.
	 * FileGuard는 생성자에서 open, 소멸자에서 close하는 RAII 스타일 클래스.
	 * */
	FileGuard file(path);

	/*
	 * 각 블럭의 생성자가 파싱합니다.
	 * make server Http->server->location
	 * */
	blocks[0] = new GeneralBlock(file.getFile());
	blocks[1] = new EventBlock(file.getFile());
	blocks[2] = new HttpBlock(file.getFile());

	/* blocks[3] = new SmtpBlock(file.getFile()); */
	/* blocks[4] = new FtpBlock(file.getFile()); */
	/* blocks[5] = new StreamBlock(file.getFile()); */
	/* std::cout<<buf<<std::endl; */
}

IBlock** Config::getBlocks() {
	return this->blocks;
}

GeneralBlock* Config::getGeneralBlock()
{
	return static_cast<GeneralBlock *>(this->blocks[0]);
}

EventBlock* Config::getEventBlock()
{
	return static_cast<EventBlock*>(this->blocks[1]);
}

HttpBlock* Config::getHTTPBlock()
{
	return static_cast<HttpBlock *>(this->blocks[2]);
}

void Config::printConfigData()
{
	std::cout<<"\033[35m"<<"Config Result"<<std::endl;

	std::cout<<"=====General====="<<std::endl;
	std::cout<<"worker_processes: "<<static_cast<GeneralBlock::generalConfig*>(blocks[0]->getConfigData())->worker_processes<<std::endl;
	std::cout<<"=====Events======"<<std::endl;
	std::cout<<"connections: "<<static_cast<EventBlock::eventConfig*>(blocks[1]->getConfigData())->worker_connections<<std::endl;


	std::cout<<"======Http======="<<std::endl;
	typedef std::vector<IHttpBlock *> t_block;
	HttpData *sb = static_cast<HttpData*>(this->getHTTPBlock()->getConfigData());
	sb->printConfig();

	for (int i = 0; i < sb->getServerBlock().size(); i++)
	{
		std::cout<<"\033[33m"<<"================Server================="<<std::endl;
		HttpServerBlock *sd = static_cast<HttpServerBlock *>(sb->getServerBlock()[i]);
		static_cast<HttpServerData *>(sd->getConfigData())->printServerDataConfig();
		for (int j = 0; j < static_cast<HttpServerData *>(sd->getConfigData())->getHttpLocationBlock().size(); j++)
		{
		std::cout<<"\033[32m"<<"==================Location=============="<<std::endl;
					static_cast<HttpLocationData *>(static_cast<HttpServerData *>(sd->getConfigData())->getHttpLocationBlock()[j]->getConfigData())->printLocationData();
		}
	}
}



/*
 * 아래 3개는 구현부가 아닙니다.
 * */
IBlock* Config::getSMTPBlock()
{
	return this->blocks[3];
}

IBlock* Config::getFTPBlock()
{
	return this->blocks[4];
}

IBlock* Config::getSTREAMBlock()
{
	return this->blocks[5];
}


Config::Config() {}
Config::~Config() 
{
	delete static_cast<GeneralBlock *>(this->blocks[0]);
	delete static_cast<EventBlock *>(this->blocks[1]);
	delete static_cast<HttpBlock *>(this->blocks[2]);
}
Config::Config(const Config&) {}
Config& Config::operator=(const Config&) {return Config::getInstance();}

