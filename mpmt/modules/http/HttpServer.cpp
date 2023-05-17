#include "HttpServer.hpp"
#include <netinet/in.h>
#include <stdexcept>


HttpServer& HttpServer::getInstance()
{
	static HttpServer instance;
	return instance;
}

int	makeSocketByLocationData(HttpLocationData &d)
{
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
	int nagle_off = d.getTcpNoDelay();
	if (setsockopt(server_fd, IPPROTO_TCP, TCP_NODELAY, &nagle_off, sizeof(nagle_off)) == -1) 
		throw(std::runtime_error("setsockopt error"));

	/* set server_addr */
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(d.getListen());

	/* bind */
	if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
		throw(std::runtime_error("bind error"));

	/* set server_fd to non-blocking */
	int flags = fcntl(server_fd, F_GETFL, 0);
	fcntl(server_fd, F_SETFL, flags | O_NONBLOCK);

	/* listen */
	if (listen(server_fd, 1024) == -1)
		throw(std::runtime_error("listen error"));
	return server_fd;
}

void HttpServer::initHttpServer() throw(std::runtime_error)
{
	this->H = static_cast<HttpBlock *>(Config::getInstance().getHTTPBlock());

	//1024로 설정. 나중에 수정 필요
	//nginx의 client_body_buffer_size, client_header_buffer_size 확인할 것.
	//https://nginx.org/en/docs/http/ngx_http_core_module.html#client_body_buffer_size
	this->HttpBuffer.resize(1024);

	std::map<int, std::vector<HttpLocationBlock *> *>& m = H->getLocationBlocksByPort();

	for (HttpBlock::locationBlocksByPortMapIter it = m.begin(); it != m.end(); it++)
	{
		int port = (*it).first;
		(*(*it).second)[0]->getLocationData().printLocationData();
		portMapPair tmp;

		tmp.first.first = makeSocketByLocationData((*(*it).second)[0]->getLocationData());
		tmp.first.second = port;
		tmp.second = ((*it).second);
		this->portMap.push_back(tmp);

		struct kevent kev;
		EV_SET(&kev, tmp.first.first, EVFILT_READ, EV_ADD, 0, 0,  ((*it).second));
		this->kevents.push_back(kev);
	}
}

bool HttpServer::isServerSocket(int socket_fd)
{
	for (int i = 0; i < this->portMap.size(); i++)
	{
		if (socket_fd == this->portMap[i].first.first)
			return true;
	}
	return false;
}

std::vector<struct kevent> & HttpServer::getKevents() { return this->kevents; }

std::vector<HttpServer::portMapPair> & HttpServer::getPortMap() { return this->portMap; }


std::string& HttpServer::getHttpBuffer() { return this->HttpBuffer; }


HttpServer::HttpServer() {}
HttpServer::~HttpServer() {}
HttpServer::HttpServer(const HttpServer &){}
HttpServer& HttpServer::operator=(const HttpServer &){ return *this; }
