#ifndef HTTP_HPP
# define HTTP_HPP

#include <netinet/tcp.h>
#include <stdexcept>
#include <sys/event.h>
#include <iostream>
#include <arpa/inet.h>
#include <fcntl.h>
#include "../../interface/IHandler.hpp"
#include "../../interface/IBlock.hpp"
#include "../../interface/IServer.hpp"

struct httpSets
{
	int port;
	

};


class HttpServer : public IServer
{
private:
	IBlock *HttpBlock;


public:
	HttpServer(IBlock *httpBlock) : HttpBlock(httpBlock) {}
	~HttpServer() {}

	/**
	 * @brief http config기반으로 http socket을 설정합니다.
	 *
	 * @throw std::runtime_error
	 */
	void initSocket() throw(std::runtime_error) 
	{
	}
};

#endif
