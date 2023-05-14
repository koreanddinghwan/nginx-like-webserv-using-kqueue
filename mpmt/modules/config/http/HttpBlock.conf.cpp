#include "HttpBlock.conf.hpp"
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

		if (buf.find("error_page") != std::string::npos)
		{
			s.splitRemoveSemiColon(buf.c_str(), ' ');
			for (int i = 1; i < s.get().size() - 1; i++)
			{
				this->confData.setErrorPage(std::atoi(s.get()[i].c_str()), s.get()[s.get().size() - 1]);
			}
		}

		if (buf.find("root") != std::string::npos)
		{
			s.splitRemoveSemiColon(buf.c_str(), ' ');
			this->confData.setRoot(s.get()[1]);
		}

		if (buf.find("sendfile") != std::string::npos) 
		{
			s.splitRemoveSemiColon(buf.c_str(), ' ');
			if (s.get()[1] == "on")
				this->confData.setSendFile(true);
		}

		if (buf.find("tcp_nodelay") != std::string::npos) 
		{
			s.splitRemoveSemiColon(buf.c_str(), ' ');
			if (s.get()[1] == "off")
				this->confData.setTcpNoDelay(false);
		}

		if (buf.find("tcp_nopush") != std::string::npos)
		{
			s.splitRemoveSemiColon(buf.c_str(), ' ');
			if (s.get()[1] == "on")
				this->confData.setTcpNoPush(true);
		}

		if (buf.find("server {") != std::string::npos)
		{
			std::cout<<"\033[32m"<<"make new server block:" <<buf<<std::endl;
			this->getConfigData().setServerBlock(new HttpServerBlock(File));
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
HttpBlock::httpData::httpData() {}

HttpBlock::httpData::~httpData() {}
std::vector<IBlock *> HttpBlock::httpData::getServerBlock() {return this->httpServerBlock;}


void HttpBlock::httpData::setServerBlock(HttpServerBlock *serverBlock)
{
	this->httpServerBlock.push_back(serverBlock);
}
