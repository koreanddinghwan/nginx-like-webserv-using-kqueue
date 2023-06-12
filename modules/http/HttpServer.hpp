#ifndef HTTPSERVER_HPP
#define HTTPSERVER_HPP

#include <vector>
#include "../config/Config.hpp"
#include "../eventLoop/Event.hpp"
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
#include <netinet/in.h>
#include <stdexcept>
#include <sys/event.h>
# define HTTPBUFFER_SIZE 130000

class HttpServer : public IServer {
private:
	HttpBlock *H;

	/**
	 * @brief http server의 kevent를 저장합니다.
	 * event loop 에서 맨 처음에 이 kevent들을 등록합니다.
	 */
	std::vector<struct kevent> kevents;


	//1024로 설정. 나중에 수정 필요
	//nginx의 client_body_buffer_size, client_header_buffer_size 확인할 것.
	//https://nginx.org/en/docs/http/ngx_http_core_module.html#client_body_buffer_size
	//default : 1024 for header
	//default : 1m(1024kb)== 1048576 for body
	char HttpBuffer[HTTPBUFFER_SIZE];

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
  void init() throw(std::runtime_error);

  std::vector<struct kevent> &getKevents();

  /**
   * @brief http server의 buffer를 리턴합니다.
   *
   * @return char * of http buffer
   */
  char* getHttpBuffer();

  unsigned int issueSessionId();

private:
  HttpServer(); 
  ~HttpServer(); 
  HttpServer(const HttpServer &hs);
  HttpServer &operator=(const HttpServer &hs);
};

#endif
