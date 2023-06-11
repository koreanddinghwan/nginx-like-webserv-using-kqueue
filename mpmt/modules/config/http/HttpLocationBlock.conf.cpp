#include "HttpLocationBlock.conf.hpp"

HttpLocationBlock::HttpLocationBlock(std::ifstream &File, HttpServerData *c) : confData(*c)
{
	this->parse(File);
}

HttpLocationBlock::HttpLocationBlock(HttpServerData *c) : confData(*c)
{}

HttpLocationBlock::~HttpLocationBlock() 
{}

IConfigData* HttpLocationBlock::getConfigData()
{
	return &this->confData;  
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

		BlockParser::httpBlockParser(buf, this->confData);
		BlockParser::httpLocationBlockParser(File, buf, this->confData);
	}
}

HttpLocationData& HttpLocationBlock::getLocationData() {
	return this->confData;
}
