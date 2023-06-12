#include "HttpServerBlock.conf.hpp"

HttpServerBlock::HttpServerBlock(std::ifstream &File, HttpData *c): serverData(*c)
{
	this->parse(File);
}

IConfigData* HttpServerBlock::getConfigData() {return &this->serverData;}
HttpServerData &HttpServerBlock::getServerData() {return this->serverData;}

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

		BlockParser::httpBlockParser(buf, *static_cast<HttpServerData *>(this->getConfigData()));
		BlockParser::httpServerBlockParser(buf, *static_cast<HttpServerData *>(this->getConfigData()));

		if (buf.find("location ") != std::string::npos)
		{
			File.seekg(cur_offset);
			this->serverData.setHttpLocationBlock((new HttpLocationBlock(File, static_cast<HttpServerData *>(this->getConfigData()))));
		}
	}
	if (this->serverData.getHttpLocationBlock().size() == 0)
		this->serverData.setHttpLocationBlock((new HttpLocationBlock(static_cast<HttpServerData *>(this->getConfigData()))));
}
