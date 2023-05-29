#include "HttpResponseInfo.hpp"

Response::Response(){};

Response::Response(const int &status) {
	setStatusCode(status);
	setStatusMsg(status);
};

Response& Response::operator=(const Response &rhs) {
	this->setStatusCode(rhs.getStatusCode());
	this->setStatusMsg(rhs.getStatusCode());
    return (*this);
}
	
void Response::setStatusMsg(int statusCode) {
	http_status_msg msg = http_status_msg(statusCode);
	this->_statusMsg = msg.getStatusMsg();
}

void Response::setStatusCode(int statusCode) {
	this->_statusCode = statusCode;
}
//객체를 받아서 스트림 그자체로 쓸 수도 있음.

void Response::setHeaders(std::string httpV) {
	this->_headers << httpV << " " << this->_statusCode << " " << this->_statusMsg << "\r\n";
	this->_headers << "Content-Type: " << "text/html" << "\r\n";
	if (this->_statusCode == 301 || this->_statusCode == 302)
		this->_headers << "Location: " << "probly file Path Variable added" << "\r\n";
	this->_body.str() != "" ? this->_headers << "Content-Length: " << this->_body.str().length() << "\r\n\r\n" : this->_headers << "\r\n\r\n";
}	
void Response::setBody(std::string body) {
	this->_body << body;
}

void Response::setBuf(){
	this->_buf << getHeader() << getBody();
}

std::string Response::getBody() const { return this->_body.str();};
std::string Response::getHeader() const { return this->_headers.str();};
std::string Response::getBuf() const {return this->_buf.str(); };
std::string Response::getStatusMsg() const { return this->_statusMsg; };
int Response::getStatusCode() const { return this->_statusCode; };