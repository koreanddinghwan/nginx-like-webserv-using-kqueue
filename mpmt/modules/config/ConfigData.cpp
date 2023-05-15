#include "ConfigData.hpp"

ConfigData::ConfigData() : client_max_body_size(1024), 
	root("html"),
	sendfile(false),
	tcp_nodelay(true),
	tcp_nopush(false),
	autoindex(false) {}

ConfigData::~ConfigData() {}
void ConfigData::operator=(ConfigData &c)
{
	this->client_max_body_size = c.getClientMaxBodySize();
	this->errorPage = c.getErrorPage();
	this->root = c.getRoot();
	this->sendfile = c.getSendFile();
	this->tcp_nodelay = c.getTcpNoDelay();
	this->tcp_nopush = c.getTcpNoPush();
}

std::map<int, std::string> ConfigData::getErrorPage() const {return this->errorPage;}
std::string ConfigData::getRoot() const {return this->root;}
bool	ConfigData::getSendFile() const {return this->sendfile;}
bool	ConfigData::getTcpNoDelay() const {return this->tcp_nodelay;}
bool	ConfigData::getTcpNoPush() const {return this->tcp_nopush;}
int		ConfigData::getClientMaxBodySize() const {return this->client_max_body_size;}
bool	ConfigData::getAutoIndex() const {return this->autoindex;}


void	ConfigData::setClientMaxBodySize(int a) {this->client_max_body_size = a;}
void	ConfigData::setErrorPage(int errCode, std::string html)
{
	this->errorPage.insert({errCode, html});
}
void	ConfigData::setRoot(std::string r) {this->root = r;}
void	ConfigData::setSendFile(bool a) {this->sendfile = a;
	}
void	ConfigData::setTcpNoDelay(bool a) {this->tcp_nodelay = a;
	}
void	ConfigData::setTcpNoPush(bool a) {this->tcp_nopush = a;
	}
void	ConfigData::setAutoIndex(bool a) { this->autoindex = a;}
