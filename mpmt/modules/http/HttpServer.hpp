#ifndef HTTP_HPP
# define HTTP_HPP

#include <netinet/tcp.h>
#include <stdexcept>
#include <sys/event.h>
#include <iostream>
#include <arpa/inet.h>
#include <fcntl.h>
#include "../../interface/IHandler.hpp"
#include "../../interface/IHttpBlock.hpp"
#include "../../interface/IServer.hpp"

class HttpServer : public IServer
{
private:
	IHttpBlock *HttpBlock;


public:
	/**
	 * @brief singleTon http Server입니다.
	 *
	 * @return static에 선언되 Httpserver를 리턴합니다.
	 */
	static HttpServer& getInstance();

	/**
	 * @brief http config기반으로 http socket을 설정합니다.
	 *
	 * @throw std::runtime_error
	 */
	void initSocket() throw(std::runtime_error); 
};

#endif
