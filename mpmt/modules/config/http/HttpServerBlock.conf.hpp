#ifndef HTTPSERVERBLOCK_HPP
# define HTTPSERVERBLOCK_HPP

#include "HttpLocationBlock.conf.hpp"
#include "../BlockParser.hpp"
#include "../data/HttpServerData.hpp"
#include "../../../lib/ft_split.hpp"
#include "../../../lib/strSplit.hpp"
#include "../../../interface/IBlock.hpp"
#include <fstream>
#include <vector>
#include <string>
#include <iostream>

/*
server { # php/fastcgi
  listen       80;
  server_name  domain1.com www.domain1.com;
  access_log   logs/domain1.access.log  main;
  root         html;

  location ~ \.php$ {

  or

server { # simple reverse-proxy
  listen       80;
  server_name  domain2.com www.domain2.com;
  access_log   logs/domain2.access.log  main;

  # serve static files
  location ~ ^/(images|javascript|js|css|flash|media|static)/  {
  location / {
*/

class HttpData;

class HttpServerBlock : public IBlock
{
private:
	HttpServerData serverData;

public:
	HttpServerBlock(std::ifstream &File, HttpData *c);
	HttpServerData& getConfigData();
	~HttpServerBlock();

private:
	void parse(std::ifstream &File);
};

#endif
