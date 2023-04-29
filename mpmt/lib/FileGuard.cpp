#include "./FileGuard.hpp"

FileGuard::FileGuard(std::string path) throw (configParseException)
{
	this->File.open(path.c_str());
	if (!File.is_open())
		throw (new configParseException);
}

std::ifstream &FileGuard::getFile() 
{
	return this->File;
}

FileGuard::~FileGuard() 
{
	this->File.close();
}

