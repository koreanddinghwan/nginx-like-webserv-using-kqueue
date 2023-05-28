#ifndef HTTP_RESPONSE_INFO_H
#define HTTP_RESPONSE_INFO_H


# include <iostream>
# include <sstream>
# include "commonHttpInfo.hpp"
#define HTTPV10 "HTTP/1.0" // HTTP/1.0
#define HTTPV11 "HTTP/1.1" // HTTP/1.1
#define HTTPV20 "HTTP/2.0" // HTTP/2.0

class Response {
	//constructor
	public:
		Response();
		Response(const int &status) {
			setStatusCode(status);
			setStatusMsg(status);
		};

		Response& operator=(const Response &rhs) {
		
		this->setStatusCode(rhs.getStatusCode());
		this->setStatusMsg(rhs.getStatusCode());
    	return (*this);
		}
	
		//setHeader
		//setStatusCode
		//setBody
		//getHeader
		//getStatusCode
		//getBody
		void setStatusMsg(int statusCode) {
			http_status_msg msg = http_status_msg(statusCode);
			this->_statusMsg = msg.getStatusMsg();
		}

		void setStatusCode(int statusCode) {
			this->_statusCode = statusCode;
		}

		void setHeaders(std::string httpV) {
			this->_headers << httpV << " " << this->_statusCode << " " << this->_statusMsg << "\r\n";
			this->_headers << "Content-Type: " << "text/html" << "\r\n";
			if (this->_statusCode == 301)
				this->_headers << "Location: " << "probly file Path Variable added";
			this->_body.str() != "" ? this->_headers << "Content-Length: " << this->_body.str().length() << "\r\n\r\n" : this->_headers << "\r\n\r\n";
		}	
		void setBody(std::string body) {
			this->_body << body;
		}

		void setBuf(){
			this->_buf << getHeader() << getBody();
		}

		std::string getBody() const { return this->_body.str();}
		std::string getHeader() const { return this->_headers.str();}
		std::string getBuf() const {return this->_buf.str(); }

		std::string getStatusMsg() const { return this->_statusMsg; }
		int getStatusCode() const { return this->_statusCode; }
		
	//destoryer
	protected:
		int _statusCode;
		std::string _statusMsg;
		std::stringstream _headers;
		std::stringstream _body;
		std::ostringstream _buf;
};

#endif