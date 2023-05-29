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
		Response(const int &status);

		Response& operator=(const Response &rhs);
	
		void setStatusMsg(int statusCode);

		void setStatusCode(int statusCode);
		//객체를 받아서 스트림 그자체로 쓸 수도 있음.

		void setHeaders(std::string httpV);	
		void setBody(std::string body);
		void setBuf();

		std::string getBody() const;
		std::string getHeader() const;
		std::string getBuf() const;

		std::string getStatusMsg() const;
		int getStatusCode() const;
		
	//destoryer
	protected:
		int _statusCode;
		std::string _statusMsg;
		std::stringstream _headers;
		std::stringstream _body;
		std::ostringstream _buf;
};

#endif