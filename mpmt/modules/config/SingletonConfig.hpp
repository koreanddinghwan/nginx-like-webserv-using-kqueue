#ifndef SINGLETONCONFIG_HPP
# define SINGLETONCONFIG_HPP

#include "../../interface/IBlock.hpp"
#include "../../interface/ISingletonConfig.hpp"
#include "../config/httpBlock.conf.hpp"
#include <fstream>

const char defaultPath[] = "./test.conf";

/* struct Route { */
/*     std::string path; */
/*     std::vector<std::string> accepted_methods; */
/*     std::string redirection; */
/*     std::string directory; */
/*     bool enable_directory_listing; */
/*     std::string default_file; */
/*     std::string cgi_extension; */
/*     bool execute_cgi; */
/*     std::string upload_directory; */
/* }; */

/* struct ServerConf { */
/* public: */
/* 	typedef typename std::vector<std::pair<int, std::pair<int, int> > > T_SocketOptionVec; */
/* 	typedef typename T_SocketOptionVec::iterator T_SocketOptIterator; */
/*     std::string host; */
/*     int port; */
/* 	int InetLayerProtocol; */
/* 	int TpLayerProtocol; */
/* 	int SkFd; */
/* 	T_SocketOptionVec SockOptVec; */
/* 	struct sockaddr_in server_addr; */
/*     std::string server_name; */
/*     std::unordered_map<int, Route> routes; */
/*     int default_route; */
/*     int client_max_body_size; */
/*     std::string error_page_404; */
/*     std::string error_page_500; */
/* }; */

class SingletonConfig : public ISingletonConfig
{
	public:
		/**
		 * @brief singleton config 객체 얻기
		 *
		 * @return signleton 객체
		 */
		static SingletonConfig& getInstance() {
			static SingletonConfig instance;
			return instance;
		}

		/**
		 * @brief config file 파싱해서 block별로 정리
		 *
		 * @param path
		 */
		void initSingletonConfig(char *path) 
		{
			confPath = path;
			File.open(path);
			/*
			 * make server http->server->location
			 * */
			File.getline(buf, 100);
			//buf에 아무것도없으면
			/* blocks[0] = webservBlock(File); */
			//buf에 event가 있으면
			/* blocks[1] = eventProcessBlock(File); */
			//buf에 http있으면
			blocks[2] = new httpBlock(File);

		}

		IBlock **getBlocks() 
		{
			return this->blocks;
		}

	


	private:
		SingletonConfig() {}
		SingletonConfig(const SingletonConfig&) {}
		SingletonConfig& operator=(const SingletonConfig&) {return SingletonConfig::getInstance();}

	private:
		char *confPath = 0;
		std::ifstream	File;
		char			buf[100];
		IBlock			*blocks[5] = {0, 0, 0, 0, 0};
};


#endif
