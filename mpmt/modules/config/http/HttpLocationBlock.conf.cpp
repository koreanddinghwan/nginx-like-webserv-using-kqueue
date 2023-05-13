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
{}
