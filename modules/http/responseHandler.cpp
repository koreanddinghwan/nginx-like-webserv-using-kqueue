#include "responseHandler.hpp"
#include "HttpreqHandler.hpp"
#include "HttpServer.hpp"


responseHandler::responseHandler(Event *e) : _event(e)
{
	(void)(_event);
	this->_res = new Response(200);
}


responseHandler::responseHandler(const int &status) {
	this->_res = new Response(status);
}

responseHandler& responseHandler::operator=(const responseHandler &rhs) {
	this->_res = rhs._res;
    return (*this);
}

responseHandler::~responseHandler() {
	delete this->_res;
}

void responseHandler::setRes(const int statusCode) {
	this->_res = new Response(statusCode);
}

void responseHandler::setResBody(std::string body) const { 
	this->_res->setBody(body); 
}

void responseHandler::setResStatusCode(const int& statusCode) const { 
	this->_res->setStatusCode(statusCode); 
}

void responseHandler::setResStatusMsg(const int& statusCode) const { 
	this->_res->setStatusMsg(statusCode); 
}

void responseHandler::setResLocation(std::string location) const { 
	this->_res->setLocation(location); 
}

void responseHandler::setResCookie(std::string cookieString) const {
	this->_res->setCookie(cookieString);
}

void responseHandler::setResAddtionalOptions(Event *event) const {
	std::string httpMethod = static_cast<HttpreqHandler *>(event->getRequestHandler())->getRequestInfo().method;
	std::string requestedResource = static_cast<HttpreqHandler *>(event->getRequestHandler())->getRequestInfo().path;
	int statusCode = this->getResStatusCode();
	
	if (static_cast<HttpreqHandler *>(event->getRequestHandler())->getHasSid())
	{
		std::string cookieString = static_cast<HttpreqHandler *>(event->getRequestHandler())->getRequestInfo().reqHeaderMap.find("Cookie")->second;
		this->setResCookie("Cookie: " + cookieString + "\r\n");
	} else {
		unsigned int tmpId = HttpServer::getInstance().issueSessionId();
		this->setResCookie("Set-Cookie: sid=" + toString(tmpId) +"; max-age:86400;\r\n");
	}

	if (httpMethod == "GET") {
		if (statusCode >= 300 && statusCode < 400)
			this->setResLocation(event->locationData->getRedirectUrl());
	} else if (httpMethod == "HEAD") {
		this->getResBody().clear();
	} else if (httpMethod == "POST") {
		if (statusCode == 201)
			this->setResLocation(requestedResource);
	} else if (httpMethod == "DELETE") {
		if(statusCode == 200) {
			this->getResBody().clear();
			this->setResBody("<!DOCTYPE html>\r\n<html lang=\"en\">\r\n<head>\r\n\t<meta charset=\"UTF-8\">\r\n\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\r\n\t<title>Page not Found</title>\r\n</head>\r\n<body>\r\n\t<h1>\r\n\t\tDelete is complete!\r\n\t</h1>\r\n</body>\r\n</html>");
		} else if (statusCode == 202) {
			this->getResBody().clear();
		} else if (statusCode == 204) {
			this->getResBody().clear();
		} 
	}
}

void responseHandler::setResHeader(std::string HttpV) const { 
	this->_res->setHeaders(HttpV); 
}

void responseHandler::setResBuf() const {
	this->_res->setBuf(); 
}

std::string& responseHandler::getResBody() const {
	return this->_res->getBody();
}

std::string& responseHandler::getResHeader() const {
	return this->_res->getHeader();
}

std::string& responseHandler::getResBuf() const {
	return this->_res->getBuf();
}

int responseHandler::getResStatusCode() const {
	return this->_res->getStatusCode();
}
std::string& responseHandler::getResStatusMsg() const {
	return this->_res->getStatusMsg();
}




void *responseHandler::handle(void *event) {

	Event *e = static_cast<Event *>(event);

	/**
	 * 1. apply event's status code
	 * */
	this->setResStatusCode(e->getStatusCode());

	/**
	 * 2. generate message
	 * */
	this->setResStatusMsg(e->getStatusCode());

	/**
	 * filling a additional header section and body
	 * */
	this->setResAddtionalOptions(e);

	
	/**
	 * 3. set resHeader
	 * */
	this->setResHeader(HTTPV11);

	/**
	 * 4. set resBuffer to send
	 * */
	this->setResBuf();

	return event;
}
