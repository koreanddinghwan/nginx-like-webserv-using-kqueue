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
			Config::getInstance().initConfig(defaultPath);
		}
		else
		{
			Config::getInstance().initConfig(av[1]);
		}
	} catch (configParseException &e) {
		throw (e);
	}
}

void WebservManager::initServers()
{
	HttpServer::getInstance().init();
}

void WebservManager::initLoop()
{
	EventLoop::getInstance().initEventLoop();
}

WebservManager::WebservManager() {}
WebservManager::~WebservManager() {}
WebservManager::WebservManager(WebservManager &){}
WebservManager& WebservManager::operator=(const WebservManager&) {return WebservManager::getInstance();}
