#ifndef HTTPSERVERBLOCK
# define HTTPSERVERBLOCK

#include "../../../interface/IBlock.hpp"
#include <fstream>
#include <vector>

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
	HttpServerBlock(std::ifstream &File) {
		this->parse(File);
	}
	~HttpServerBlock() {}

private:
	std::vector<IBlock *> httpLocationBlock;

	void parse(std::ifstream &File) 
	{
		/*
		 * while 'location' directives in buf,
		 * {
		 * 		httpServerBlock.push(new httpLocationBlock(File));
		 * }
		 * */
	}
};

#endif
