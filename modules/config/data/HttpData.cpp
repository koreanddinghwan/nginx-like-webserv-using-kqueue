#include "HttpData.hpp"
/*
 * http block 전용 data getter setter
 * */
HttpData::HttpData() {}
HttpData::~HttpData() {}
HttpData::HttpData(HttpData &c)
{
	(*this) = c;
}
void HttpData::operator=(HttpData &c) {
	this->setClientMaxBodySize(c.getClientMaxBodySize());
	this->copyErrorPage(c.getErrorPage());
	this->setRoot(c.getRoot());
	this->setAutoIndex(c.getAutoIndex());
	this->setSendFile(c.getSendFile());
	this->setTcpNoDelay(c.getTcpNoDelay());
	this->setTcpNoPush(c.getTcpNoPush());
	for (size_t i = 0 ; i < c.getIndex().size(); i++)
	{
		this->setIndex(c.getIndex()[i]);
	}
}

std::vector<HttpServerBlock *> &HttpData::getServerBlock() {return this->httpServerBlock;}


void HttpData::setServerBlock(HttpServerBlock *serverBlock)
{
	this->httpServerBlock.push_back(serverBlock);
}

