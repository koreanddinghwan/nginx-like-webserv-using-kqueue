#ifndef HTTP_HPP
# define HTTP_HPP

#include <stdexcept>
#include <sys/event.h>
#include <iostream>
#include "../../interface/IHandler.hpp"
#include "../../interface/IBlock.hpp"
#include "../../interface/IServer.hpp"

class HttpServer : public IServer
{
private:
	IBlock *HttpBlock;
	IHandler *RequestHandler;
	IHandler *ResponseHandler;


public:
	HttpServer
		(IBlock *httpBlock) : 
		HttpBlock(httpBlock)
	{
		//do something with httpblock
	}
	~HttpServer() {}
	void run() throw(std::runtime_error) {
		//
	}
};

#endif
