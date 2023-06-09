#include "HttpServerData.hpp"
#include "HttpConfigData.hpp"
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

std::vector<std::string> &HttpServerData::getServerNames() {return this->server_names;}

std::vector<HttpLocationBlock *> &HttpServerData::getHttpLocationBlock() {return this->httpLocationBlock;}

std::vector<HttpLocationData *> &HttpServerData::getLocationDatas() {return this->_LocationDatas;}

void HttpServerData::setListen(int port) {this->listen  = port;}

void HttpServerData::setServerName(std::string name) {
	this->server_names.push_back(name);
}

void HttpServerData::setHttpLocationBlock(HttpLocationBlock *n) {
	this->httpLocationBlock.push_back(n);
}

std::string HttpServerData::getUploadStore() {return this->upload_store;}

void HttpServerData::setUploadStore(std::string upload_store) {this->upload_store = upload_store;}
