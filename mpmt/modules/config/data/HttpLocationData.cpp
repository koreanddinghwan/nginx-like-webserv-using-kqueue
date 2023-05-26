#include "HttpLocationData.hpp"
#include "HttpServerData.hpp"
#include <cstring>

HttpLocationData::HttpLocationData(): 
	uri_match_mode(PREFIX),
	return_status(-1)
{
	std::cout<<"Default location block"<<std::endl;
	//allow all methods default
	std::memset(&(this->lim_methods), ALLOW, sizeof(limited_methods));
}

HttpLocationData::~HttpLocationData() {}
HttpLocationData::HttpLocationData(HttpServerData &c)
{
	//allow all methods default
	std::memset(&(this->lim_methods), ALLOW, sizeof(limited_methods));
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
	for (int i = 0 ; i < c.getIndex().size(); i++)
	{
		this->setIndex(c.getIndex()[i]);
	}
	this->setListen(c.getListen());
	for (int i = 0; i < c.getServerNames().size(); i++)
		this->setServerName(c.getServerNames()[i]);
}


int HttpLocationData::getUriMatchMode() {return this->uri_match_mode;}
std::string& HttpLocationData::getUri() {return this->uri;}
std::string& HttpLocationData::getProxyPass() {return this->proxy_pass;}
std::string& HttpLocationData::getCgiPass() {return this->cgi_pass;}
std::string& HttpLocationData::getFastcgiPass() {return this->fastcgi_pass;}
int HttpLocationData::getReturnStatus() {return this->return_status;}
std::string& HttpLocationData::getRedirectUrl() {return this->redirect_url;}


void HttpLocationData::setUriMatchMode(enum e_uri_match_mode e) { this->uri_match_mode = e;}
void HttpLocationData::setUri(std::string u) { this->uri = u;}
void HttpLocationData::setProxyPass(std::string p) { this->proxy_pass = p;}
void HttpLocationData::setCgiPass(std::string c) { this->cgi_pass = c;}
void HttpLocationData::setFastcgiPass(std::string f) { this->fastcgi_pass = f;}
void HttpLocationData::setReturnStatus(int r) { this->return_status = r;}
void HttpLocationData::setRedirectUrl(std::string r) { this->redirect_url = r;}

/**
 * @brief limit the methods that this location could retrieve.
 * 
 * nginx documentation 
 * @link https://nginx.org/en/docs/http/ngx_http_core_module.html#limit_except
 *
 * @param r the string to check
 */
void HttpLocationData::setLimitedMethods(std::vector<std::string> r)
{
	//deny all methods first
	std::memset(&(this->lim_methods), DENY, sizeof(limited_methods));
	for (int i = 1; i <r.size(); i++)
	{
		//allowing get also allow head. check nginx documentation that representate `limit_except`
		if (r[i] == "GET")
		{
			this->lim_methods.methods[HEAD] = ALLOW;
			this->lim_methods.methods[GET] = ALLOW;
		}
		else if (r[i] == "HEAD")
			this->lim_methods.methods[HEAD] = ALLOW;
		else if (r[i] == "POST")
			this->lim_methods.methods[POST] = ALLOW;
		else if (r[i] == "PUT")
			this->lim_methods.methods[PUT] = ALLOW;
		else if (r[i] == "DELETE")
			this->lim_methods.methods[DELETE] = ALLOW;
		else if (r[i] == "MKCOL")
			this->lim_methods.methods[MKCOL] = ALLOW;
		else if (r[i] == "COPY")
			this->lim_methods.methods[COPY] = ALLOW;
		else if (r[i] == "MOVE")
			this->lim_methods.methods[MOVE] = ALLOW;
		else if (r[i] == "OPTIONS")
			this->lim_methods.methods[OPTIONS] = ALLOW;
		else if (r[i] == "PROPFIND")
			this->lim_methods.methods[PROPFIND] = ALLOW;
		else if (r[i] == "PROPPATCH")
			this->lim_methods.methods[PROPPATCH] = ALLOW;
		else if (r[i] == "LOCK")
			this->lim_methods.methods[LOCK] = ALLOW;
		else if (r[i] == "UNLOCK")
			this->lim_methods.methods[UNLOCK] = ALLOW;
		else if (r[i] == "PATCH")
			this->lim_methods.methods[PATCH] = ALLOW;
	}
}


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
	std::cout<<"allowed_methods: ";
	for (int i = 0; i < 14; i++)
	{
		std::string text;
		if (lim_methods.methods[i] == ALLOW)
			text = "ALLOW";
		else
			text = "DENY";

		if (i == GET)
			std::cout<<"GET: "<<text<<std::endl;
		else if (i == HEAD)
			std::cout<<"HEAD: "<<text<<std::endl;
		else if (i == POST)
			std::cout<<"POST: "<<text<<std::endl;
		else if (i == PUT)
			std::cout<<"PUT: "<<text<<std::endl;
		else if (i == DELETE)
			std::cout<<"DELETE: "<<text<<std::endl;
		else if (i == MKCOL)
			std::cout<<"MKCOL: "<<text<<std::endl;
		else if (i == COPY)
			std::cout<<"COPY: "<<text<<std::endl;
		else if (i == MOVE)
			std::cout<<"MOVE: "<<text<<std::endl;
		else if (i == OPTIONS)
			std::cout<<"OPTIONS: "<<text<<std::endl;
		else if (i == PROPFIND)
			std::cout<<"PROPFIND: "<<text<<std::endl;
		else if (i == PROPPATCH)
			std::cout<<"PROPPATCH: "<<text<<std::endl;
		else if (i == LOCK)
			std::cout<<"LOCK: "<<text<<std::endl;
		else if (i == UNLOCK)
			std::cout<<"UNLOCK: "<<text<<std::endl;
		else if (i == PATCH)
			std::cout<<"PATCH: "<<text<<std::endl;
	}
}
