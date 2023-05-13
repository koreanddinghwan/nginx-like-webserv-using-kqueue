#include "./WebservManager.hpp"

WebservManager& WebservManager::getInstance() {
	static WebservManager instance;
	return instance;
}

/**
 * @brief config를 초기화합니다
 * 싱글톤 객체인 config를 초기화합니다.
 * @param ac : main's ac
 * @param av : main's av
 */
void WebservManager::initConfig(int ac, char **av) const throw (std::runtime_error, configParseException)
{
	char defaultPath[14] = "./nginx.conf";
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
}

/**
 * @brief nginx의 event loop
 * worker process가 설정되었다면 구현이 살짝 달라질수도...?
 */
void WebservManager::initLoop()
{
	EventLoop::getInstance().initEventLoop();
}

WebservManager::WebservManager() {}
WebservManager::~WebservManager() {}
WebservManager::WebservManager(WebservManager &){}
WebservManager& WebservManager::operator=(const WebservManager&) {return WebservManager::getInstance();}
