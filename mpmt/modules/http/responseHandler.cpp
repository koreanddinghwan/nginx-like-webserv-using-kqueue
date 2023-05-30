#include "responseHandler.hpp"


responseHandler::responseHandler() {
};


responseHandler::responseHandler(const int &status) {
	this->_res = new Response(status);
};

responseHandler& responseHandler::operator=(const responseHandler &rhs) {
	this->_res = rhs._res;
    return (*this);
};

responseHandler::~responseHandler() {
	delete this->_res;
};

void responseHandler::setRes(const int statusCode) {
	this->_res = new Response(statusCode);
};

void responseHandler::setResBody(std::string body) const { 
	this->_res->setBody(body); 
};

void responseHandler::setResStatusCode(const int& statusCode) const { 
	this->_res->setStatusCode(statusCode); 
};

void responseHandler::setResStatusMsg(const int& statusCode) const { 
	this->_res->setStatusMsg(statusCode); 
};

void responseHandler::setResLocation(std::string location) const { 
	this->_res->setLocation(location); 
};
void responseHandler::setResHeader(std::string HttpV) const { 
	this->_res->setHeaders(HttpV); 
};

void responseHandler::setResBuf() const {
	this->_res->setBuf(); 
};

std::string& responseHandler::getResBody() const {
	return this->_res->getBody();
};

std::string& responseHandler::getResHeader() const {
	return this->_res->getHeader();
};

std::string& responseHandler::getResBuf() const {
	return this->_res->getBuf();
};



void *responseHandler::handle(void *event) {

	Event *e = static_cast<Event *>(event);

	std::cout<<"statussss:"<<e->getStatusCode()<<std::endl;

	/**
	 * 1. apply event's status code
	 * */
	this->_res->setStatusCode(e->getStatusCode());

	/**
	 * 2. generate message
	 * */
	this->setResStatusMsg(e->getStatusCode());

	/**
	 * if need redirection, set location header
	 * */
	if (e->getStatusCode() >= 300 && e->getStatusCode() < 400)
		this->setResLocation(e->locationData->getRedirectUrl());

	std::cout << "=====================\n" << this->getResBody() << "=====================\n" << std::endl;

	/**
	 * 3. set resHeader
	 * */
	this->setResHeader(HTTPV11);
	std::cout << "=====================\n" << this->getResHeader() << "=====================\n" << std::endl;

	/**
	 * 4. set resBuffer to send
	 * */
	this->setResBuf();
	std::cout << "=====================\n" << this->getResBuf() << "\n=====================" << std::endl;
}
