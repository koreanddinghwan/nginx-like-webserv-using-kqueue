#include "HttpLocationData.hpp"
#include "HttpServerData.hpp"

HttpLocationData::HttpLocationData(): 
	uri_match_mode(PREFIX),
	return_status(-1)
{}

HttpLocationData::~HttpLocationData() {}
HttpLocationData::HttpLocationData(HttpServerData &c)
{
	uri_match_mode = PREFIX;
	return_status = -1;
	(*this) = c;
}

void HttpLocationData::operator=(HttpServerData &c)
{
	this->setClientMaxBodySize(c.getClientMaxBodySize());
	this->copyErrorPage(c.getErrorPage());
	this->setRoot(c.getRoot());
	this->setAutoIndex(c.getAutoIndex());
	this->setSendFile(c.getSendFile());
	this->setTcpNoDelay(c.getTcpNoDelay());
	this->setTcpNoPush(c.getTcpNoPush());
	this->setListen(c.getListen());
	for (int i = 0; i < c.getServerNames().size(); i++)
		this->setServerName(c.getServerNames()[i]);
}


int HttpLocationData::getUriMatchMode() {return this->uri_match_mode;}
std::string HttpLocationData::getUri() {return this->uri;}
std::string HttpLocationData::getProxyPass() {return this->proxy_pass;}
std::string HttpLocationData::getFastcgiPass() {return this->fastcgi_pass;}
int HttpLocationData::getReturnStatus() {return this->return_status;}
std::string HttpLocationData::getRedirectUrl() {return this->redirect_url;}


void HttpLocationData::setUriMatchMode(enum e_uri_match_mode e) { this->uri_match_mode = e;}
void HttpLocationData::setUri(std::string u) { this->uri = u;}
void HttpLocationData::setProxyPass(std::string p) { this->proxy_pass = p;}
void HttpLocationData::setFastcgiPass(std::string f) { this->fastcgi_pass = f;}
void HttpLocationData::setReturnStatus(int r) { this->return_status = r;}
void HttpLocationData::setRedirectUrl(std::string r) { this->redirect_url = r;}

void HttpLocationData::printLocationData() {
	this->printServerDataConfig();
	std::cout<<"uri_match_mode: ";	
	if (uri_match_mode == PREFIX)
		std::cout<<"PREFIX"<<std::endl;
	else
		std::cout<<"EXACT"<<std::endl;

	std::cout<<"uri: "<<uri<<std::endl;
	std::cout<<"proxy_pass: "<<proxy_pass<<std::endl;
	std::cout<<"fastcgi_pass: "<<fastcgi_pass<<std::endl;
	std::cout<<"return_status: "<<return_status<<std::endl;
	std::cout<<"redirect_url: "<<redirect_url<<std::endl;
}
