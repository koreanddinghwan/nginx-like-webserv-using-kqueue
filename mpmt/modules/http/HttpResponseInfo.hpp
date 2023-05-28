#ifndef HTTP_RESPONSE_INFO_H
#define HTTP_RESPONSE_INFO_H


# include <iostream>
# include <sstream>

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
		std::string getStatusMsg() const { return this->_statusMsg; }
		int getStatusCode() const { return this->_statusCode; }
		
	//destoryer
	protected:
		int _statusCode;
		std::string _statusMsg;
		std::string _headers;
		std::string _body;
		std::ostringstream _buf;
};

#endif