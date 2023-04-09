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

		void run() 
		{
			WebservManager::getInstance().run();
		}


	private:
		WebServ() {}
};

#endif
