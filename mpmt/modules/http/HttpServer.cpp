#include "HttpServer.hpp"
#include <netinet/in.h>


HttpServer& HttpServer::getInstance()
{
	static HttpServer instance;
	return instance;
}

void HttpServer::initSocket() throw(std::runtime_error)
{
	/* get generalBlock*/
	G = static_cast<GeneralBlock *>(Config::getInstance().getGeneralBlock());
	
	/* print general data*/
	std::cout<<"Worker_process"<<static_cast<GeneralBlock::generalConfig *>(G->getConfigData())->worker_processes<<std::endl;

	/* get eventBlock*/
	E = static_cast<EventBlock *>(Config::getInstance().getEventBlock());
	std::cout<<"Worker_connections"<<static_cast<EventBlock::eventConfig *>(E->getConfigData())->worker_connections<<std::endl;

	/* get httpBlock*/
	H = static_cast<HttpBlock *>(Config::getInstance().getHTTPBlock());

	/* get httpServerData */
	std::cout<<"httpServerBlock"<<std::endl;
	std::vector<IHttpBlock *> hsb = static_cast<HttpData *>(H->getConfigData())->getServerBlock();

	for (int i = 0; i <  hsb.size(); i++)
	{
		HttpServerData *hsd = static_cast<HttpServerData *>(hsb[i]->getConfigData());
		hsd->printServerDataConfig();

		this->socketMap.insert(getSocketMapPair(hsd));

		/* for (int j = 0; j < hsd->getHttpLocationBlock().size(); j++) */
		/* { */
		/* 	static_cast<HttpLocationData *>(hsd->getHttpLocationBlock()[j]->getConfigData())->printLocationData(); */
		/* } */
	}
}

HttpServer::socketMapPair HttpServer::getSocketMapPair(HttpServerData *hsd)
{
	socketMapPair pair;
	int server_fd;
	struct sockaddr_in server_addr;


	/* create socket */
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		throw(std::runtime_error("socket error"));


	/* set socket option */
	int opt = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
		throw(std::runtime_error("setsockopt error"));


	int keep_alive = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_KEEPALIVE, &keep_alive, sizeof(keep_alive)) == -1)
		throw(std::runtime_error("setsockopt error"));


	// Disable Nagle algorithm
	int nagle_off = static_cast<HttpData* >(this->H->getConfigData())->getTcpNoDelay();
	if (setsockopt(server_fd, IPPROTO_TCP, TCP_NODELAY, &nagle_off, sizeof(nagle_off)) == -1) 
		throw(std::runtime_error("setsockopt error"));

	/* set server_addr */
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(hsd->getListen());

	/* bind */
	if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
		throw(std::runtime_error("bind error"));


	/* set server_fd to non-blocking */
	int flags = fcntl(server_fd, F_GETFL, 0);
	fcntl(server_fd, F_SETFL, flags | O_NONBLOCK);


	/* listen */
	if (listen(server_fd, 10) == -1)
		throw(std::runtime_error("listen error"));

	pair.first = server_fd;
	pair.second = hsd;
	return pair;
}
