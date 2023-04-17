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

		/**
		 * @brief WebServ를 초기화합니다
		 * 싱글톤 객체인 config과 webserv manager를 초기화합니다.
		 * @param ac : main's ac
		 * @param av : main's av
		 */
		void init(int ac, char ** av) const
		{
			if (ac > 1)
			{
				std::cerr<<"Please input 1 config file or nothing"<<std::endl;
				exit(1);
			}
			//config 모듈 초기화
			SingletonConfig::getInstance().initSingletonConfig(av[1]);

			//webserv가 처리할 http, smtp, ftp 등 서버 초기화
			WebservManager::getInstance().initServers();
		}

		/**
		 * @brief WebservManager의 이벤트루프를 구동합니다.
		 */
		void run() const
		{
			WebservManager::getInstance().initLoop();
		}


	private:
		WebServ() {}
};

#endif
