#ifndef WEBSERV_HPP
# define WEBSERV_HPP

#include "./interface/IWebServ.hpp"
#include "./modules/manager/WebservManager.hpp"
#include "exceptions/configParseException.hpp"
#include <exception>
#include <iostream>
#include <stdexcept>

char defaultPath[14] = "./nginx.conf";
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
		void init(int ac, char ** av) const throw (std::runtime_error, configParseException)
		{
			if (ac > 2)
				throw std::runtime_error("Please input 1 config file route or nothing");

			try {
				if (ac == 1)
				{
					std::cout<<"Set config file as default path..."<<std::endl;
					Config::getInstance().initConfig(defaultPath);
				}
				else
				{
					std::cout<<"Set config file as av[1] path..."<<av[1]<<std::endl;
					Config::getInstance().initConfig(av[1]);
				}
			} catch (configParseException &e) {
				throw (e);
			}

			//webserv가 처리할 http, smtp, ftp 등 서버 초기화
			try {
				WebservManager::getInstance().initServers();
			} catch (std::exception &e) {
				throw(e);
			}
		}

		/**
		 * @brief WebservManager의 이벤트루프.
		 */
		void run() const
		{
			WebservManager::getInstance().initLoop();
		}


	private:
		WebServ() {}
};

#endif
