#include "./lib.hpp"
#include "strSplit.hpp"

strSplit::strSplit() {}

std::pair<std::string, std::string> strSplit::split(std::string str, const char del)
{
	this->splited.first.clear();
	this->splited.second.clear();

	int i = 0;
	while (str[i] == 32)
		i++;
	while (str[i] != del)
	{
		if (str[i] != 32)
			splited.first.push_back(str[i++]);
		else
			i++;
	}

	while (str[i] == 32)
		i++;

	while (str[i] != ';')
		splited.second.push_back(str[i++]);

	return this->splited;
}

strSplit::~strSplit() {}

