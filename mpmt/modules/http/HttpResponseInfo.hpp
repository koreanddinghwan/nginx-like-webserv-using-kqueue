#ifndef HTTP_RESPONSE_INFO_H
#define HTTP_RESPONSE_INFO_H


# include <iostream>
# include <sstream>
# include <cstdlib>

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

		void setHeaders(std::string const &httpV);	
		void setBody(std::string const &body);
		void setLocation(std::string const &location);
		void setCgiBody(std::string const &substr);
		void setBuf();

		std::string& getBody();
		std::string& getHeader();
		std::string& getBuf();

		std::string& getStatusMsg();
		int getStatusCode() const;
		
	//destoryer
	protected:
		int _statusCode;
		std::string _statusMsg;
		std::string _location;
		std::string _headers;
		std::string _body;
		std::string _buf;
};

#endif