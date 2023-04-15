#ifndef WEBSERV_HPP
# define WEBSERV_HPP

#include "./interface/IWebServ.hpp"
#include "./modules/manager/WebservManager.hpp"

/**
 * @brief singleton
 */
class WebServ : public IWebServ
{
	public:
		static WebServ& getInstance()
		{
			static WebServ instance;
			return instance;
		}

		void init(int ac, char ** av)
		{
			if (ac > 1)
			{
				std::cerr<<"Please input 1 config file or nothing"<<std::endl;
				exit(1);
			}
			//config 모듈 초기화
			SingletonConfig::getInstance().initSingletonConfig(av[1]);
			WebservManager::getInstance().initServers();
		}

		void run() 
		{
			WebservManager::getInstance().initLoop();
		}


	private:
		WebServ() {}
};

#endif
