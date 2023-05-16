#include "HttpServer.hpp"


HttpServer& HttpServer::getInstance()
{
	static HttpServer instance;
	return instance;
}

void HttpServer::initSocket() throw(std::runtime_error)
{
	


}

