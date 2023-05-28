#ifndef RESPONSEHANDLER_HPP
# define RESPONSEHANDLER_HPP

#include "../eventLoop/Event.hpp"
#include "HttpRequestInfo.hpp"
#include "../../interface/IHandler.hpp"
#include "HttpResponseInfo.hpp"
#include <fstream>

class responseHandler : public IHandler
{
public:
	responseHandler() {
	}
	~responseHandler() {}

	void setRes(const int statusCode) {
		this->_res = new Response(statusCode);
	}

	void setResBody(std::string body) const { this->_res->setBody(body); }
	void setResHeader(std::string HttpV) const { this->_res->setHeaders(HttpV); }
	void setResBuf() const {this->_res->setBuf(); }
	std::string getResBody() const {return this->_res->getBody();}
	std::string getResHeader() const {return this->_res->getHeader();}
	std::string getResBuf() const {return this->_res->getBuf();}
	void *handle(void *event) {
		Event *e = static_cast<Event *>(event);
		std::ifstream test;

		test.open("index.html");
		this->setRes(404);
		this->setResBody("is somethind text");
		std::cout << "=====================\n" << this->getResBody() << "=====================\n" << std::endl;
		this->setResHeader(HTTPV11);
		std::cout << "=====================\n" << this->getResHeader() << "=====================\n" << std::endl;
		this->setResBuf();
		
		std::cout << "=====================\n" << this->getResBuf() << "\n=====================" << std::endl;

		//Response Res = new Response(e->getStatusCode());  
		/* Res->buf << reqInfo->HttpVersion << " " << Res->statusCode << " " << statusCodeMsg << "\r\n";
		if (Res->body) {
			Res->buf << "Content-Length: " << Res->body << "\r\n" 
			Res->buf << "Content-Type: " << "text/html" 
			Res->Buf << "\r\n\r\n"
			Res->Buf << Res->body << "\r\n"
		}
		else 
			Res->Buf << "\r\n\r\n" */
		
	}
private:
	Response* _res;
};

#endif
