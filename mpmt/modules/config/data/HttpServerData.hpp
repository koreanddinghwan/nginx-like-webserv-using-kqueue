#ifndef HTTPSERVERDATA_HPP
# define HTTPSERVERDATA_HPP

#include "HttpData.hpp"

class HttpLocationData;
class HttpLocationBlock;
class HttpData;

class HttpServerData : public HttpData
{
	private:
		int listen;
		std::string upload_store;
		std::vector<std::string> server_names;
		std::vector<HttpLocationBlock *> httpLocationBlock;
		std::vector<HttpLocationData *> _LocationDatas;

	public:
		HttpServerData();
		~HttpServerData();
		HttpServerData& operator=(HttpData &c);
		HttpServerData(HttpData &c);
 
		int  getListen();
		std::vector<std::string> &getServerNames();
		std::vector<HttpLocationBlock *> &getHttpLocationBlock();
		std::vector<HttpLocationData *> &getLocationDatas();
		void setListen(int port);
		void setServerName(std::string name);
		void setHttpLocationBlock(HttpLocationBlock *n);
		void printServerDataConfig();

		std::string getUploadStore();
		void setUploadStore(std::string upload_store);
};

#endif
