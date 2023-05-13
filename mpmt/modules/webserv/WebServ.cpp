#include "./webserv.hpp"

WebServ& WebServ::getInstance()
{
	static WebServ instance;
	return instance;
}

/**
 * @brief webserve manager를 기반으로 webserv를 구동한다.
 *
 * @param ac
 * @param av
 */
void WebServ::run(int ac, char **av) const
{
	WebservManager::getInstance().initConfig(ac, av);
	WebservManager::getInstance().initLoop();
}

WebServ::WebServ() {}
