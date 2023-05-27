#ifndef HTTPSERVERDATA_HPP
# define HTTPSERVERDATA_HPP

#include "HttpData.hpp"
#include "../../../interface/IHttpBlock.hpp"

class HttpLocationBlock;
class HttpData;

class HttpServerData : public HttpData
{
	private:
		int listen;
		std::vector<std::string> server_names;
		std::vector<IHttpBlock *> httpLocationBlock;
	public:
		HttpServerData();
		~HttpServerData();
		HttpServerData& operator=(HttpData &c);
		HttpServerData(HttpData &c);
 
		int  getListen();
		std::vector<std::string> &getServerNames();
		std::vector<IHttpBlock *> &getHttpLocationBlock();
		void setListen(int port);
		void setServerName(std::string name);
		void setHttpLocationBlock(IHttpBlock *n);
		void printServerDataConfig();
};

#endif
