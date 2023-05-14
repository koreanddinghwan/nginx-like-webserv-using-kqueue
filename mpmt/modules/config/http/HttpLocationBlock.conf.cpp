#include "HttpLocationBlock.conf.hpp"

HttpLocationBlock::HttpLocationBlock(std::ifstream &File) {
	this->parse(File);
}

HttpLocationBlock::~HttpLocationBlock() 
{}

HttpLocationBlock::httpLocationData& HttpLocationBlock::getConfigData()
{
	return this->confData;  
}

void HttpLocationBlock::parse(std::ifstream &File)
{
	std::string	buf;
	ft_split	s;


	while (buf.find("}") == std::string::npos)
	{
		if (File.eof())
			break;
		std::getline(File, buf);

		std::cout<<"current:"<<buf<<std::endl;
		if (buf.find("location") != std::string::npos)
		{
			s.split(buf.c_str(), ' ');

			//check exact match
			if (s.get()[1] == "=")
			{
				confData.uri_match_mode = EXACT;
				confData.uri.assign(s.get()[2]);
			}
			else
			{
				confData.uri_match_mode = PREFIX;
				confData.uri.assign(s.get()[1]);
			}
		}

		if (buf.find("root") != std::string::npos)
		{
			s.split(buf.c_str(), ' ');
			confData.root.assign(s.get()[1].substr(0, s.get()[1].size() - 1));
		}

		if (buf.find("proxy_pass") != std::string::npos)
		{
			s.split(buf.c_str(), ' ');
			confData.proxy_pass.assign(s.get()[1].substr(0, s.get()[1].size() - 1));
		}

		if (buf.find("fastcgi_pass") != std::string::npos)
		{
			s.split(buf.c_str(), ' ');
			confData.fastcgi_pass.assign(s.get()[1].substr(0, s.get()[1].size() - 1));
		}

		if (buf.find("return") != std::string::npos)
		{
			s.split(buf.c_str(), ' ');
			confData.return_status = std::atoi(s.get()[1].c_str());
			confData.redirect_url.assign(s.get()[2].substr(0, s.get()[2].size() - 1));
		}

		/*
		 * getcwd -> opendir -> readdir
		 * */
		if (buf.find("auto_index") != std::string::npos)
		{
			s.split(buf.c_str(), ' ');
			if (s.get()[2] == "on;")
				confData.auto_index = true;
			else
				confData.auto_index = false;
		}
	}

	std::cout<<"end of location block"<<std::endl;
}
