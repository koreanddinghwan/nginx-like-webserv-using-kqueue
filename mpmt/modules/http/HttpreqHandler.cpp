#include "HttpreqHandler.hpp"

void *HttpreqHandler::handle(void *e) 
{
	/*
	 * 화이팅 !!@obsovo
	 * */
	return e;
}

HttpreqHandler::HttpreqHandler()
{
	info = new httpRequestInfo();
}

HttpreqHandler::~HttpreqHandler()
{
	delete this->info;
}
