#include "HttpServerData.hpp"
#include "ConfigData.hpp"
#include "HttpData.hpp"
#include "HttpLocationData.hpp"
#include <vector>

HttpServerData::HttpServerData() :listen(80) {}

HttpServerData::~HttpServerData() {}

HttpServerData& HttpServerData::operator=(HttpData &c)
{
	this->setClientMaxBodySize(c.getClientMaxBodySize());
	this->copyErrorPage(c.getErrorPage());
	this->setRoot(c.getRoot());
	this->setAutoIndex(c.getAutoIndex());
	this->setSendFile(c.getSendFile());
	this->setTcpNoDelay(c.getTcpNoDelay());
	this->setTcpNoPush(c.getTcpNoPush());
	return (*this);
}

HttpServerData::HttpServerData(HttpData &c)
{
	(*this) = c;
}

int  HttpServerData::getListen() {return this->listen;}
std::vector<std::string> HttpServerData::getServerNames() {return this->server_names;}
std::vector<IBlock *> HttpServerData::getHttpLocationBlock() {return this->httpLocationBlock;}
void HttpServerData::setListen(int port) {this->listen  = port;}
void HttpServerData::setServerName(std::string name) {
	this->server_names.push_back(name);
}
void HttpServerData::setHttpLocationBlock(IBlock *n) {this->httpLocationBlock.push_back(n);}
void HttpServerData::printServerDataConfig() {
	this->printConfig();
	std::cout<<"listen: "<<listen<<std::endl;
	for (int i = 0; i < server_names.size(); i++)
	{
		std::cout<<"server name: "<<server_names[i]<<std::endl;
	}
}