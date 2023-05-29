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

	//change a value below,
	//
	//this->setRes(e->somthing->statusCode);
	//if (e->something->content for responseBody)
	//	this->setResBody(e->something->content);
	//this->_res->setStatusCode(e->getStatusCode());
	//this->_res->setStatusMsg(e->getStatusCode());
	//if (e->something->redirectLocation)
	//	this->setResLocation(e->something->redirectLoaction);


	std::cout << "=====================\n" << this->getResBody() << "=====================\n" << std::endl;
	this->setResHeader(HTTPV11);
	std::cout << "=====================\n" << this->getResHeader() << "=====================\n" << std::endl;
	this->setResBuf();
	std::cout << "=====================\n" << this->getResBuf() << "\n=====================" << std::endl;
}
