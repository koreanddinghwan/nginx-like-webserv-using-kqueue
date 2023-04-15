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
