#ifndef HTTPSERVERDATA_HPP
# define HTTPSERVERDATA_HPP

#include "../../../interface/IBlock.hpp"
#include "../data/ConfigData.hpp"

class HttpLocationBlock;
class HttpData;

class HttpServerData : public ConfigData
{
	private:
		int listen;
		std::vector<std::string> server_names;
		std::vector<IBlock *> httpLocationBlock;
	public:
		HttpServerData();
		~HttpServerData();
		HttpServerData& operator=(HttpData &c);
		HttpServerData(HttpData &c);
 
		int  getListen();
		std::vector<std::string> getServerNames();
		std::vector<IBlock *> getHttpLocationBlock();
		void setListen(int port);
		void setServerName(std::string name);
		void setHttpLocationBlock(IBlock *n);
		void printServerDataConfig();
};

#endif
