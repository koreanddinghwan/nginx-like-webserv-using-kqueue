#include "./HttpBlock.conf.hpp"
#include "HttpServerBlock.conf.hpp"
#include <string>
#include <vector>

HttpBlock::HttpBlock(std::ifstream &File) 
{
		this->parse(File);
}

HttpBlock::~HttpBlock() {
	for (int i = 0; i < this->confData.getServerBlock().size(); i++)
		delete  static_cast<HttpServerBlock *>(this->confData.getServerBlock()[i]);
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
			this->confData.setClientMaxBodySize(std::atoi(s.get()[1].c_str()));
		}

		if (buf.find("sendfile") != std::string::npos)
		{
			s.split(buf.c_str(), ' ');
			if (s.get()[1] == "on;")
				this->confData.setSendFile(true);
			else
				this->confData.setSendFile(false);
		}

		if (buf.find("tcp_nopush") != std::string::npos)
		{
			s.split(buf.c_str(), ' ');
			if (s.get()[1] == "on;")
				this->confData.setTcpNoPush(true);
			else
				this->confData.setTcpNoPush(false);
		}

		if (buf.find("server {") != std::string::npos)
		{
			std::cout<<"\033[32m"<<"make new server block:" <<buf<<std::endl;
			this->confData.getServerBlock().push_back(new HttpServerBlock(File));
		}
	}
}

HttpBlock::httpData& HttpBlock::getConfigData()
{
	return (this->confData);
}



/*
 * http block 전용 data getter setter
 * */
HttpBlock::httpData::httpData(): client_max_body_size(-1), sendfile(false), tcp_nopush(false) {}
HttpBlock::httpData::~httpData() {}

void	HttpBlock::httpData::setClientMaxBodySize(int a) {this->client_max_body_size = a;}
void	HttpBlock::httpData::setSendFile(bool a) {this->sendfile = a;}
void	HttpBlock::httpData::setTcpNoPush(bool a) {this->tcp_nopush = a;}
int		HttpBlock::httpData::getClientMaxBodySize() const {return this->client_max_body_size;}
bool	HttpBlock::httpData::getSendFile() const {return this->sendfile;}	
bool	HttpBlock::httpData::getTcpNoPush() const {return this->tcp_nopush;}	
std::vector<IBlock *> HttpBlock::httpData::getServerBlock() {return this->httpServerBlock;}
