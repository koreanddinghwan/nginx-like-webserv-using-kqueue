#ifndef WEBSERV_MANAGER_HPP
# define WEBSERV_MANAGER_HPP

#include "../../interface/IWebservManager.hpp"
#include "../../interface/IConfig.hpp"
#include "../../interface/IServer.hpp"
#include "../config/SingletonConfig.hpp"

/**
 * @brief singltton
 */
class WebservManager: public IWebservManager
{
	public:
		static WebservManager& getInstance() {
			static WebservManager instance;
			return instance;
		}

		void run()
		{
			SingletonConfig::getInstance().getBlocks();
		}



	private:
		WebservManager() {}

		WebservManager(WebservManager &){}
		WebservManager& operator=(const WebservManager&) {return WebservManager::getInstance();}

	private:
		IServer *Servers[5];
};


#endif
