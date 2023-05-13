#include "HttpLocationBlock.conf.hpp"

HttpLocationBlock::HttpLocationBlock(std::ifstream &File) {
	this->parse(File);
}

HttpLocationBlock::~HttpLocationBlock() 
{}

void HttpLocationBlock::parse(std::ifstream &File)
{}
