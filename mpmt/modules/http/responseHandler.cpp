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

	this->setRes(302);
	this->setResBody("bodybody");
	std::cout << "=====================\n" << this->getResBody() << "=====================\n" << std::endl;
	this->setResHeader(HTTPV11);
	std::cout << "=====================\n" << this->getResHeader() << "=====================\n" << std::endl;
	this->setResBuf();
	std::cout << "=====================\n" << this->getResBuf() << "\n=====================" << std::endl;
}