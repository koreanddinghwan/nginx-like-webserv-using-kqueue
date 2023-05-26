#ifndef WEBSERV_MANAGER_HPP
# define WEBSERV_MANAGER_HPP

#include "../eventLoop/EventLoop.hpp"
#include "../eventLoop/Event.hpp"
#include "../../interface/IWebservManager.hpp"
#include "../../interface/IConfig.hpp"
#include "../../interface/IServer.hpp"
#include <stdexcept>
#include <sys/event.h>
#include "../config/Config.hpp"
#include "../http/HttpServer.hpp"

/**
 * @brief singleton
 */
class WebservManager: public IWebservManager
{
	public:
		static WebservManager& getInstance();

		/**
		 * @brief config를 초기화합니다
		 * 싱글톤 객체인 config를 초기화합니다.
		 * @param ac : main's ac
		 * @param av : main's av
		 */
		void initConfig(int ac, char **av) const throw (std::runtime_error, configParseException);


		void initServers();


		/**
		 * @brief nginx의 event loop
		 * worker process가 설정되었다면 구현이 살짝 달라질수도...?
		 */
		void initLoop();

	private:
		WebservManager(); 
		~WebservManager(); 
		WebservManager(WebservManager &);
		WebservManager& operator=(const WebservManager&);
};


#endif
