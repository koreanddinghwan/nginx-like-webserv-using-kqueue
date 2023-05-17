#include "./WebservManager.hpp"

WebservManager& WebservManager::getInstance() {
	static WebservManager instance;
	return instance;
}

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
		Config::getInstance().printConfigData();
	} catch (configParseException &e) {
		throw (e);
	}
}

void WebservManager::initServers()
{
	HttpServer::getInstance().initHttpServer();
}

void WebservManager::initLoop()
{
	EventLoop::getInstance().initEventLoop();
}

WebservManager::WebservManager() {}
WebservManager::~WebservManager() {}
WebservManager::WebservManager(WebservManager &){}
WebservManager& WebservManager::operator=(const WebservManager&) {return WebservManager::getInstance();}
