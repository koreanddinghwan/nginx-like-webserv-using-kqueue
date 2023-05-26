#ifndef HTTPSERVER_HPP
#define HTTPSERVER_HPP

#include "../config/Config.hpp"
#include "../eventLoop/Event.hpp"
#include "../config/http/HttpBlock.conf.hpp"
#include "../eventLoop/EventManager.hpp"
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

class HttpServer : public IServer {
private:
	HttpBlock *H;
	HttpBlock::locationDatasByPortMap *locationDatasByPortMap;
	std::vector<struct kevent> kevents;
	std::vector<int> serverSocketFd;
	/*     clientFd, obejct*/
	/* std::map<int, request> requestMap; */

	//1024로 설정. 나중에 수정 필요
	//nginx의 client_body_buffer_size, client_header_buffer_size 확인할 것.
	//https://nginx.org/en/docs/http/ngx_http_core_module.html#client_body_buffer_size
	char HttpBuffer[1024];

	//is it needed?
	//anyway, resize in init function to 1024
	std::string stringBuffer;

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

  /**
   * @brief server socket인지 확인합니다.
   *
   * @param socket_fd
   *
   * @return 
   */
  bool isServerSocket(int socket_fd);

  void makeSocketByLocationData(std::vector<HttpLocationData *> *m, Event *e);
 
  std::vector<struct kevent> &getKevents();
  HttpBlock::locationDatasByPortMap *getLocationDatasByPortMap(); 
  char* getHttpBuffer();
  std::string &getStringBuffer();

private:
  HttpServer(); 
  ~HttpServer(); 
  HttpServer(const HttpServer &hs);
  HttpServer &operator=(const HttpServer &hs);
};

#endif
