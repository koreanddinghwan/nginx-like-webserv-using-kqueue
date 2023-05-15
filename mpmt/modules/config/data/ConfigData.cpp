#include "ConfigData.hpp"

ConfigData::ConfigData() : 
	client_max_body_size(1024), 
	root("html"),
	sendfile(false),
	tcp_nodelay(true),
	tcp_nopush(false),
	autoindex(false) {}

ConfigData::~ConfigData() {}

ConfigData& ConfigData::operator=(ConfigData &c)
{
	std::cout<<"operator called"<<std::endl;
	this->client_max_body_size = c.getClientMaxBodySize();
	this->errorPage = c.getErrorPage();
	this->root.assign(c.getRoot());
	this->autoindex = c.getAutoIndex();
	this->sendfile = c.getSendFile();
	this->tcp_nodelay = c.getTcpNoDelay();
	this->tcp_nopush = c.getTcpNoPush();
	return (*this);
}

ConfigData::ConfigData(ConfigData &c)
{
	std::cout<<"copy init"<<std::endl;
	(*this) = c;
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
	std::pair<int, std::string> tmp;

	tmp.first = errCode;
	tmp.second = html;
	this->errorPage.insert(tmp);
}

void	ConfigData::copyErrorPage(errorMap m)
{
	for (errorMap::iterator it = m.begin(); it != m.end(); it++)
	{
		setErrorPage((*it).first, (*it).second);
	}
}

void	ConfigData::setRoot(std::string r) {this->root = r;}
void	ConfigData::setSendFile(bool a) {this->sendfile = a;
	}
void	ConfigData::setTcpNoDelay(bool a) {this->tcp_nodelay = a;
	}
void	ConfigData::setTcpNoPush(bool a) {this->tcp_nopush = a;
	}
void	ConfigData::setAutoIndex(bool a) { this->autoindex = a;}

void	ConfigData::printConfig() {
	std::cout<<"client max body size: "<<this->getClientMaxBodySize()<<std::endl;

	std::cout<<"error page: "<<std::endl;
	errorMap em = this->getErrorPage();
	for (errorMap::iterator it = em.begin(); it != em.end(); it++)
	{
		std::cout<<"["<<(*it).first<<","<<"]"<<"["<<(*it).second<<"]"<<std::endl;
	}

	std::cout<<"root: "<<this->getRoot()<<std::endl;

	std::cout<<"sendfile: "<<this->getSendFile()<<std::endl;
	std::cout<<"tcpnodelay: "<<this->getTcpNoDelay()<<std::endl;
	std::cout<<"tcpnopush: "<<this->getTcpNoPush()<<std::endl;
	std::cout<<"autoindex: "<<this->getAutoIndex()<<std::endl;
}
