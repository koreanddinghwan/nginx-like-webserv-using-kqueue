#include "HttpServerBlock.conf.hpp"
#include "HttpLocationBlock.conf.hpp"

HttpServerBlock::HttpServerBlock(std::ifstream &File)
{
	this->parse(File);
}

HttpServerBlock::HttpServerData& HttpServerBlock::getConfigData() {return this->serverData;}

HttpServerBlock::~HttpServerBlock() {}

void HttpServerBlock::parse(std::ifstream &File) 
{
	std::string	buf;
	strSplit	spl;
	ft_split	s;

	while (buf.find("}") == std::string::npos)
	{
		if (File.eof())
			break;
		std::getline(File, buf);

		std::cout<<"current:"<<buf<<std::endl;
		if (buf.find("location ") != std::string::npos)
		{
			std::cout<<"\033[31m"<<"make new location block"<<buf<<std::endl;
			this->serverData.httpLocationBlock.push_back(new HttpLocationBlock(File));
		}

		//found listen port
		if (buf.find("listen") != std::string::npos)
			this->serverData.listen = atoi(spl.split(buf, ' ').second.c_str());
		if (buf.find("server_name") != std::string::npos)
		{
			s.split(buf.c_str(), ' ');
			for (int i = 1; i < s.get().size(); i++)
			{
				if (i == s.get().size() - 1)
					this->serverData.server_names.push_back(s.get()[i].substr(0, s.get()[1].size() - 1));
				else
					this->serverData.server_names.push_back(s.get()[i]);
			}
		};
		if (buf.find("root") != std::string::npos)
		{
			s.split(buf.c_str(), ' ');
			this->serverData.root.assign(s.get()[1].substr(0, s.get()[1].size() - 1));
		}
		if (buf.find("error_page") != std::string::npos)
		{
			s.split(buf.c_str(), ' ');
			this->serverData.errorPage.assign(s.get()[1].substr(0, s.get()[1].size() - 1));
		}
	}

	std::cout<<"end of server block"<<std::endl;

	/*
	 * while 'location' directives in buf,
	 * {
	 * 		httpServerBlock.push(new httpLocationBlock(File));
	 * }
	 * */
}
