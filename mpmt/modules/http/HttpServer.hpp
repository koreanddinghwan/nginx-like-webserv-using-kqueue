#ifndef HTTPSERVER_HPP
#define HTTPSERVER_HPP

#include "../config/Config.hpp"
#include "../config/http/HttpBlock.conf.hpp"
#include "../config/GeneralBlock.conf.hpp"
#include "../config/EventBlock.conf.hpp"
#include "../../interface/IHandler.hpp"
#include "../../interface/IServer.hpp"
#include <arpa/inet.h>
#include <fcntl.h>
#include <iostream>
#include <map>
#include <netinet/tcp.h>
#include <stdexcept>
#include <sys/event.h>

class HttpServer : public IServer {
private:
	GeneralBlock *G;
	EventBlock *E;
	HttpBlock *H;

	typedef std::pair<int, HttpServerData *> socketMapPair;
	typedef std::map<int, HttpServerData *>::iterator socketMapIter;
	typedef std::map<int, HttpServerData *>::const_iterator socketMapConstIter;


	/*
	 * socket_fd : socket file descriptor for this server
	 * locationDatas : location data for this server
	 * */
	std::map<int, HttpServerData *> socketMap;

public:
  /**
   * @brief singleTon http Server입니다.
   *
   * @return static에 선언되 Httpserver를 리턴합니다.
   */
  static HttpServer &getInstance();

  /**
   * @brief http config기반으로 http socket을 설정합니다.
   *
   * @throw std::runtime_error
   */
  void initSocket() throw(std::runtime_error);


private:
  HttpServer() {}
  ~HttpServer() {}
  HttpServer(const HttpServer &hs);
  HttpServer &operator=(const HttpServer &hs);


  socketMapPair getSocketMapPair(HttpServerData *hsd);
};

#endif
