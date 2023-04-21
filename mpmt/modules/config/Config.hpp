#ifndef SINGLETONCONFIG_HPP
# define SINGLETONCONFIG_HPP

#include "../../exceptions/configParseException.hpp"
#include "../../interface/IBlock.hpp"
#include "eventBlock.conf.hpp"
#include "generalBlock.conf.hpp"
#include "../../interface/IConfig.hpp"
#include "./http/HttpBlock.conf.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include "../../lib/RaIIFile.hpp"

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

/**
 * @brief config module, singleton
 *
 * 설정파일을 읽어 블록단위로 설정합니다.
 * general, event, Http, smtp, ftp, stream 총 6개의 블록이 있으며, 필수 구현부는 general, event, Http입니다.
 */
class Config : public IConfig
{
	public:
		/**
		 * @brief singleton config 객체 얻기
		 *
		 * @return signleton 객체
		 */
		static Config& getInstance() {
			static Config instance;
			return instance;
		}

		/**
		 * @brief config file 파싱해서 block별로 정리
		 *
		 * @param path
		 */
		void initConfig(std::string path) throw (configParseException)
		{
			RaIIFile file(path);

			/*
			 * make server Http->server->location
			 * */
			blocks[0] = new generalBlock(file.getFile());
			blocks[1] = new eventBlock(file.getFile());
			blocks[2] = new HttpBlock(file.getFile());
			/* std::cout<<buf<<std::endl; */

			std::cout<<static_cast<generalBlock::generalConfig *>(blocks[0]->getConfigData())->worker_processes<<std::endl;
			std::cout<<static_cast<eventBlock::eventConfig *>(blocks[1]->getConfigData())->worker_connections<<std::endl;
		}

		IBlock *getGeneralBlock()
		{
			return this->blocks[0];
		}

		IBlock *getEventBlock()
		{
			return this->blocks[1];
		}

		IBlock *getHttpBlock()
		{
			return this->blocks[2];
		}

		/* IBlock *getSMTPBlock() */
		/* {} */

		/* IBlock *getFTPBlock() */
		/* {} */

		/* IBlock *getSTREAMBlock() */
		/* {} */


	private:
		Config() {}
		~Config() 
		{
			/* delete static_cast<generalBlock *>(this->blocks[0]); */
			/* delete static_cast<eventBlock *>(this->blocks[1]); */
			delete static_cast<HttpBlock *>(this->blocks[2]);
		}
		Config(const Config&) {}
		Config& operator=(const Config&) {return Config::getInstance();}

	private:
		std::string		buf;
		IBlock			*blocks[5];
};


#endif
