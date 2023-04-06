#ifndef CONFIG_HPP
# define CONFIG_HPP

#include "../../interface/IModule.hpp"
#include <stdexcept>


#include <iostream>
#include <netinet/in.h>
#include <fstream>
#include <string>
#include <sys/socket.h>
#include <vector>
#include <unordered_map>

struct Route {
    std::string path;
    std::vector<std::string> accepted_methods;
    std::string redirection;
    std::string directory;
    bool enable_directory_listing;
    std::string default_file;
    std::string cgi_extension;
    bool execute_cgi;
    std::string upload_directory;
};

struct ServerConf {
public:
	typedef typename std::vector<std::pair<int, std::pair<int, int> > > T_SocketOptionVec;
	typedef typename T_SocketOptionVec::iterator T_SocketOptIterator;
    std::string host;
    int port;
	int InetLayerProtocol;
	int TpLayerProtocol;
	int SkFd;
	T_SocketOptionVec SockOptVec;
	struct sockaddr_in server_addr;
    std::string server_name;
    std::unordered_map<int, Route> routes;
    int default_route;
    int client_max_body_size;
    std::string error_page_404;
    std::string error_page_500;
};

struct HttpConfigs {
	std::vector<ServerConf> servers;
};

const char defaultPath[] = "./test.conf";

class Config
{
private:
	std::string			ConfigPath;
	std::ifstream		File;
	HttpConfigs			httpConfs;

	//server setting
	//socket setting

public:
	Config(const char *path) 
	{
		//set default path
		if (!path)
			this->ConfigPath = defaultPath;
		else
			this->ConfigPath.assign(path);
	}

	/**
	 * @brief do configuration
	 *
	 * @throw std::runtime_error
	 */
	void configuration() throw(std::runtime_error)
	{
		//parse
		//


		

	}

	

	~Config() 
	{
		if (File.is_open())
			File.close();
	}
};

#endif
