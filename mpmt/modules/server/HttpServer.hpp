#ifndef HTTP_HPP
# define HTTP_HPP

#include <stdexcept>
#include <sys/event.h>
#include <iostream>
#include "../http/requestHandler.hpp"
#include "../../interface/IServer.hpp"

class HttpServer : public IServer
{
private:
	int kq_fd;
	//servermodules
	requestHandler *RequestHandler;


public:
	HttpServer(
			//server modules
			IHandler *reqHandler,
			IHandler *resHandler
			) 
	{
		this->kq_fd = kqueue();
		if (kq_fd == -1) {
			std::cerr << "Failed to create kqueue\n";
			exit(1);
		}
	}
	~HttpServer() {}
	void run() throw(std::runtime_error) {}
};

#endif
