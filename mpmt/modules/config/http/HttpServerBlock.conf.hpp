#ifndef HTTPSERVERBLOCK
# define HTTPSERVERBLOCK

#include "../../../lib/ft_split.hpp"
#include "../../../lib/strSplit.hpp"
#include "../../../interface/IBlock.hpp"
#include "HttpLocationBlock.conf.hpp"
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


class HttpServerBlock : public IBlock
{
public:
	class HttpServerData : public ConfigData
	{
		private:
			int listen;
			std::vector<std::string> server_names;
			std::vector<IBlock *> httpLocationBlock;
		public:
			HttpServerData();
			~HttpServerData();

			int  getListen();
			std::vector<std::string> getServerNames();
			std::vector<IBlock *> getHttpLocationBlock();
			void setListen(int port);
			void setServerName(std::string name);
			void setHttpLocationBlock(HttpLocationBlock *n);
	};

private:
	HttpServerData serverData;

public:
	HttpServerBlock(std::ifstream &File, ConfigData &c);
	HttpServerData& getConfigData();
	~HttpServerBlock();

private:
	void parse(std::ifstream &File);

};

#endif
