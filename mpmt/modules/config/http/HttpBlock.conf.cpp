#include "./HttpBlock.conf.hpp"
#include "HttpServerBlock.conf.hpp"
#include <string>
#include <vector>

HttpBlock::HttpBlock(std::ifstream &File) 
{
		this->parse(File);
}

HttpBlock::~HttpBlock() {
	for (std::vector<IBlock *>::iterator it = this->confData.httpServerBlock.begin(); it != this->confData.httpServerBlock.end(); it++)
	{
		delete static_cast<HttpServerBlock *>(*it);
	}
}


void HttpBlock::parse(std::ifstream &File) 
{
	int cur_offset = File.tellg();
	std::string buf;
	ft_split s;
	std::getline(File, buf);

	//http 찾을때까지 점프
	while (buf.find("http") == std::string::npos)
		std::getline(File, buf);

	//http 찾으면 } 찾을때까지 점프
	while (buf.find("}") == std::string::npos)
	{
		if (File.eof())
			break;

		std::getline(File, buf);
		if (buf.find("client_max_body_size") != std::string::npos)
		{
			s.split(buf.c_str(), ' ');
			this->confData.client_max_body_size = std::atoi(s.get()[1].c_str());
		}

		if (buf.find("sendfile") != std::string::npos)
		{
			s.split(buf.c_str(), ' ');
			if (s.get()[1] == "on;")
				this->confData.sendfile = true;
			else
				this->confData.sendfile = false;
		}

		if (buf.find("tcp_nopush") != std::string::npos)
		{
			s.split(buf.c_str(), ' ');
			if (s.get()[1] == "on;")
				this->confData.tcp_nopush = true;
			else
				this->confData.tcp_nopush = false;
		}

		if (buf.find("server {") != std::string::npos)
		{
			std::cout<<"\033[32m"<<"make new server block:" <<buf<<std::endl;
			this->confData.httpServerBlock.push_back(new HttpServerBlock(File));
		}
	}

	std::cout<<"\033[0m"<<"client_max_body_size : "<<this->confData.client_max_body_size<<std::endl;

}

HttpBlock::httpData& HttpBlock::getConfigData()
{
	return (this->confData);
}
