#include "httpException.hpp"

ExceptionMessageFactory::ExceptionMessageFactory(){}
ExceptionMessageFactory::~ExceptionMessageFactory(){}
ExceptionMessageFactory::ExceptionMessageFactory(const ExceptionMessageFactory &other)
{
	(void)other;
}
ExceptionMessageFactory &ExceptionMessageFactory::operator=(const ExceptionMessageFactory &other)
{
	(void)other;
	return *this;
}

ExceptionMessageFactory &ExceptionMessageFactory::getInstance()
{
	static ExceptionMessageFactory instance;
	return instance;
}

const std::string &ExceptionMessageFactory::getExceptionMessage(int statusCode) const
{
	if (statusCode >= 500)
		return this->_500_505[statusCode - 500];
	else if (statusCode >= 400)
		return this->_400_417[statusCode - 400];
	else if (statusCode >= 300)
		return this->_300_307[statusCode - 300];
	else if (statusCode >= 200)
		return this->_200_206[statusCode - 200];
	else 
		return this->_100_101[statusCode - 100];
}


HttpException::HttpException(int statusCode): 
	statusCode(statusCode), 
	message(ExceptionMessageFactory::getInstance().getExceptionMessage(statusCode))
{} 

const char *HttpException::what() const throw()
{
	return message.c_str();
}

int HttpException::getStatusCode() const
{
	return statusCode;
}
