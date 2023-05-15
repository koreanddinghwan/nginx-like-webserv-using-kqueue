#include "HttpServerBlock.conf.hpp"
#include "HttpLocationBlock.conf.hpp"
#include "../Parser.cpp"

HttpServerBlock::HttpServerBlock(std::ifstream &File, ConfigData &c)
{
	static_cast<ConfigData>(serverData) = c;
	this->parse(File);
}

HttpServerBlock::HttpServerData& HttpServerBlock::getConfigData() {return this->serverData;}

HttpServerBlock::~HttpServerBlock() {}

void HttpServerBlock::parse(std::ifstream &File) 
{
	std::string	buf;
	strSplit	spl;

	while (buf.find("}") == std::string::npos)
	{
		if (File.eof())
			break;
		std::getline(File, buf);

		Parser::httpBlockParser(buf, this->getConfigData());
		Parser::httpServerBlockParser(buf, this->getConfigData());

		std::cout<<"current:"<<buf<<std::endl;
		if (buf.find("location ") != std::string::npos)
		{
			std::cout<<"\033[31m"<<"make new location block"<<buf<<std::endl;
			this->serverData.setHttpLocationBlock(new HttpLocationBlock(File, this->getConfigData()));
		}
	}
	std::cout<<"end of server block"<<std::endl;
}


HttpServerBlock::HttpServerData::HttpServerData() :listen(80) 
{
	server_names.push_back("");
}

HttpServerBlock::HttpServerData::~HttpServerData() {}

int  HttpServerBlock::HttpServerData::getListen() {return this->listen;}
std::vector<std::string> HttpServerBlock::HttpServerData::getServerNames() {return this->server_names;}
std::vector<IBlock *> HttpServerBlock::HttpServerData::getHttpLocationBlock() {return this->httpLocationBlock;}
void HttpServerBlock::HttpServerData::setListen(int port) {this->listen  = port;}
void HttpServerBlock::HttpServerData::setServerName(std::string name) {this->server_names.push_back(name);}
void HttpServerBlock::HttpServerData::setHttpLocationBlock(HttpLocationBlock *n) {this->httpLocationBlock.push_back(n);}
