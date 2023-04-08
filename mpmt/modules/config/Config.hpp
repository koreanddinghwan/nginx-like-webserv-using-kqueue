#ifndef CONFIG_HPP
# define CONFIG_HPP

#include "../../interface/IBlock.hpp"
#include <stdexcept>
#include <string>
#include <iostream>
#include <netinet/in.h>
#include <fstream>
#include <string>
#include <sys/socket.h>
#include <vector>
#include <unordered_map>
#include "../../interface/IServer.hpp"
#include "../config/httpBlock.conf.hpp"

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

const char defaultPath[] = "./test.conf";

class Config
{
private:
	std::string		ConfigPath;
	std::ifstream	File;
	char			buf[100];
	IBlock			*blocks[5];
	/*
	 * 0. webserv conf block
	 * 1. event processing conf block
	 * 2. http block
	 * 3. mail block
	 * 4. stream block
	 * */



public:
	Config(const char *path) 
	{
		//set default path
		if (!path)
			this->ConfigPath = defaultPath;
		else
			this->ConfigPath.assign(path);
		this->File.open(path);
	}

	IBlock **getBlocks() 
	{
		/*
		 * make server http->server->location
		 * */
		File.getline(buf, 100);
		//buf에 아무것도없으면
		/* blocks[0] = webservBlock(File); */
		//buf에 event가 있으면
		/* blocks[1] = eventProcessBlock(File); */
		//buf에 http있으면
		blocks[2] = new httpBlock(File);


		return blocks;
	}
	

	~Config() 
	{
		if (File.is_open())
			File.close();
		for (int i = 0; i < 5; i++)
			delete (blocks + i);
	}
};

#endif
