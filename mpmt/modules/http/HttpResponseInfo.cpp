#include "HttpResponseInfo.hpp"
#include <algorithm>

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

std::string toString(int n) {
	std::string tmp;

	while (n > 0) {
		tmp.push_back(n%10 + '0');
		n /= 10;
	}
	reverse(tmp.begin(), tmp.end());
	return (tmp); 
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
	this->_headers = httpV + " " + toString(this->_statusCode) + " " + this->_statusMsg + "\r\n" + "Content-Type: " + "text/html" + "\r\n";
	if ((this->_statusCode == 301 && this->_location != "") || (this->_statusCode == 302 && this->_location != ""))
		this->_headers += "Location: " + this->_location + "\r\n";
	this->_headers += "Content-Length: ";
	this->_body != "" ? this->_headers += toString(this->_body.size()) + "\r\n\r\n" : this->_headers += "0\r\n\r\n";
}	
void Response::setLocation(std::string location) {
	this->_location = location;
};

void Response::setBody(std::string body) {
	this->_body += body;
}

void Response::setBuf(){
	this->_buf += getHeader();
	this->_buf += getBody();
}

std::string& Response::getBody(){ return this->_body;};
std::string& Response::getHeader() { return this->_headers;};
std::string& Response::getBuf() {return this->_buf; };
std::string& Response::getStatusMsg() { return this->_statusMsg; };
int Response::getStatusCode() const { return this->_statusCode; };