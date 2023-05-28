#include "MethodFactory.hpp"

bool MethodFactory::isMethod(std::string& method) const
{
	for (int i = 0; i < 14; i++)
	{
		if (this->_methods[i] == method)
			return (true);
	}
	return (false);
}

int MethodFactory::getMethodIndex(std::string method) const
{
	for (int i = 0; i < 14; i++)
	{
		if (this->_methods[i] == method)
			return (i);
	}
	return (-1);
}

const std::string& MethodFactory::getMethod(int index) const
{
	return (this->_methods[index]);
}

MethodFactory &MethodFactory::getInstance()
{
	static MethodFactory instance;
	return instance;
}

MethodFactory::MethodFactory()
{
	MethodFactory::_methods[0] = "GET";
	MethodFactory::_methods[1] = "HEAD";
	MethodFactory::_methods[2] = "POST";
	MethodFactory::_methods[3] = "PUT";
	MethodFactory::_methods[4] = "DELETE";
	MethodFactory::_methods[5] = "MKCOL";
	MethodFactory::_methods[6] = "COPY";
	MethodFactory::_methods[7] = "MOVE";
	MethodFactory::_methods[8] = "OPTIONS";
	MethodFactory::_methods[9] = "PROPFIND";
	MethodFactory::_methods[10] = "PROPPATCH";
	MethodFactory::_methods[11] = "LOCK";
	MethodFactory::_methods[12] = "UNLOCK";
	MethodFactory::_methods[13] = "PATCH";
}

MethodFactory::~MethodFactory()
{}

MethodFactory::MethodFactory(const MethodFactory &other)
{
	(void)other;
}

MethodFactory &MethodFactory::operator=(const MethodFactory &other)
{
	(void)other;
	return (*this);
}
