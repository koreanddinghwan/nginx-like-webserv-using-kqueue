#include "HttpConfigData.hpp"

HttpConfigData::HttpConfigData() : 
	client_max_body_size(0), 
	root("html"),
	sendfile(false),
	tcp_nodelay(true),
	tcp_nopush(false),
	autoindex(false) {}

HttpConfigData::~HttpConfigData() {}

HttpConfigData& HttpConfigData::operator=(HttpConfigData &c)
{
	this->client_max_body_size = c.getClientMaxBodySize();
	this->copyErrorPage(c.getErrorPage());
	this->root.assign(c.getRoot());
	this->autoindex = c.getAutoIndex();
	this->sendfile = c.getSendFile();
	this->tcp_nodelay = c.getTcpNoDelay();
	this->tcp_nopush = c.getTcpNoPush();
	for (int i = 0 ; i < c.getIndex().size(); i++)
	{
		this->index.push_back(c.getIndex()[i]);
	}
	return (*this);
}

HttpConfigData::HttpConfigData(HttpConfigData &c)
{
	(*this) = c;
}

const std::map<int, std::string> &HttpConfigData::getErrorPage() const {return this->errorPage;}
std::string HttpConfigData::getRoot() const {return this->root;}
bool	HttpConfigData::getSendFile() const {return this->sendfile;}
bool	HttpConfigData::getTcpNoDelay() const {return this->tcp_nodelay;}
bool	HttpConfigData::getTcpNoPush() const {return this->tcp_nopush;}
int		HttpConfigData::getClientMaxBodySize() const {return this->client_max_body_size;}
bool	HttpConfigData::getAutoIndex() const {return this->autoindex;}
std::vector<std::string> HttpConfigData::getIndex() const {return this->index;}

void	HttpConfigData::setClientMaxBodySize(int a) {this->client_max_body_size = a;}
void	HttpConfigData::setErrorPage(int errCode, std::string html)
{
	std::pair<int, std::string> tmp;

	tmp.first = errCode;
	tmp.second = html;
	this->errorPage.insert(tmp);
}

void	HttpConfigData::setRoot(std::string r) {this->root = r;}
void	HttpConfigData::setSendFile(bool a) {this->sendfile = a;
	}
void	HttpConfigData::setTcpNoDelay(bool a) {this->tcp_nodelay = a;
	}
void	HttpConfigData::setTcpNoPush(bool a) {this->tcp_nopush = a;
	}
void	HttpConfigData::setAutoIndex(bool a) { this->autoindex = a;}
void	HttpConfigData::setIndex(std::string i) { this->index.push_back(i);}


void	HttpConfigData::copyErrorPage(errorMap m)
{
	for (errorMap::iterator it = m.begin(); it != m.end(); it++)
	{
		setErrorPage((*it).first, (*it).second);
	}
}

