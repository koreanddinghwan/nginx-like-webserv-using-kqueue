#include "./RaIIFile.hpp"

RaIIFile::RaIIFile(std::string path) throw (configParseException)
{
	this->File.open(path);
	if (!File.is_open())
		throw (new configParseException);
}

std::ifstream &RaIIFile::getFile() 
{
	return this->File;
}

RaIIFile::~RaIIFile() 
{
	this->File.close();
}

