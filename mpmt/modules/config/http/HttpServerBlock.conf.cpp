#include "HttpServerBlock.conf.hpp"

HttpServerBlock::HttpServerBlock(std::ifstream &File, HttpData *c): serverData(*c)
{
	this->parse(File);
}

HttpServerData& HttpServerBlock::getConfigData() {return this->serverData;}

HttpServerBlock::~HttpServerBlock() {}

void HttpServerBlock::parse(std::ifstream &File) 
{
	int cur_offset;
	std::string	buf;
	strSplit	spl;

	while (buf.find("}") == std::string::npos)
	{
		if (File.eof())
			break;
		cur_offset = File.tellg();
		std::getline(File, buf);

		BlockParser::httpBlockParser(buf, this->getConfigData());
		BlockParser::httpServerBlockParser(buf, this->getConfigData());

		std::cout<<"current:"<<buf<<std::endl;
		if (buf.find("location ") != std::string::npos)
		{
			File.seekg(cur_offset);
			std::cout<<"\033[31m"<<"make new location block"<<buf<<std::endl;
			this->serverData.setHttpLocationBlock(static_cast<IBlock *>(new HttpLocationBlock(File, &(this->getConfigData()))));
		}
	}
	if (this->serverData.getHttpLocationBlock().size() == 0)
		this->serverData.setHttpLocationBlock(static_cast<IBlock *>(new HttpLocationBlock(&(this->getConfigData()))));
	std::cout<<"end of server block"<<std::endl;
}
