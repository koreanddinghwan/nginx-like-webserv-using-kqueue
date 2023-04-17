#ifndef WEBSERV_MANAGER_HPP
# define WEBSERV_MANAGER_HPP

#include "../../interface/IWebservManager.hpp"
#include "../../interface/IConfig.hpp"
#include "../../interface/IServer.hpp"
#include <sys/event.h>
#include "../config/SingletonConfig.hpp"
#include "../http/HttpServer.hpp"

/**
 * @brief singleton
 */
class WebservManager: public IWebservManager
{
	private:
		int kq_fd;
		IServer *httpServer;


	public:
		static WebservManager& getInstance() {
			static WebservManager instance;
			return instance;
		}

		/**
		 * @brief config 싱골톤 객체에서 설정블록을 불러와 server를 초기화합니다.
		 */
		void initServers()
		{
			/*
			 * do something with general(multi processing), event block(worker thread)
			 * */
			this->httpServer = new HttpServer(SingletonConfig::getInstance().getHttpBlock());
			
			/*
			 * do something
			 *
			 * */
		}

		/**
		 * @brief nginx의 event loop
		 * worker process가 설정되었다면 구현이 살짝 달라질수도...?
		 */
		void initLoop()
		{
			this->kq_fd = kqueue();
			if (kq_fd == -1) {
				std::cerr << "Failed to create kqueue\n";
				exit(1);
			}
			/*
			 * make loop by while, do something with blocks
			 * */
		}



	private:
		WebservManager() {}
		~WebservManager() {
			delete static_cast<HttpServer *>(httpServer);
		}
		WebservManager(WebservManager &){}
		WebservManager& operator=(const WebservManager&) {return WebservManager::getInstance();}
};


#endif
