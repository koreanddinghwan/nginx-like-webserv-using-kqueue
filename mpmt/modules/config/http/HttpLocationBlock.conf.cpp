#include "HttpLocationBlock.conf.hpp"

HttpLocationBlock::HttpLocationBlock(std::ifstream &File, HttpServerData *c) : confData(*c)
{
	this->parse(File);
}

HttpLocationBlock::~HttpLocationBlock() 
{}

HttpLocationData& HttpLocationBlock::getConfigData()
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
		BlockParser::httpBlockParser(buf, this->confData);
		BlockParser::httpLocationBlockParser(buf, this->confData);
	}
	std::cout<<"end of location block"<<std::endl;
}
