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

public:
	/*                    socket_fd, port number location blocks                */
	typedef std::pair<std::pair<int, int>, std::vector<HttpLocationData *>* > portMapPair;
  typedef std::vector<HttpLocationData *>* HTTPEventUdataType;

private:
	HttpBlock *H;

	std::vector<struct kevent> kevents;
	std::vector<portMapPair> portMap;

	/* buffer to process http request, */
	std::string HttpBuffer;

	/* http reqeust parsing class */

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
  void initHttpServer() throw(std::runtime_error);


  /**
   * @brief server socket인지 확인합니다.
   *
   * @param socket_fd
   *
   * @return 
   */
  bool isServerSocket(int socket_fd);

  /**
   * @brief kevent로부터 locationDataVector를 얻어옵니다.
   *
   * @param udata
   *
   * @return HttpLocationData vector를 리턴합니다.
   */
  static HTTPEventUdataType getLocationDataVectorFromKevent(struct kevent *e); 


  std::vector<struct kevent> &getKevents();
  std::vector<portMapPair> &getPortMap();
  std::string& getHttpBuffer();


private:
  HttpServer(); 
  ~HttpServer(); 
  HttpServer(const HttpServer &hs);
  HttpServer &operator=(const HttpServer &hs);
};

#endif
