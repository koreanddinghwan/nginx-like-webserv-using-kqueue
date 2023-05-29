#include "httpException.hpp"

ExceptionMessageFactory::ExceptionMessageFactory() {

	this->_100_101[0] = "Continue";
	this->_100_101[1] = "Switching Protocols";

	this->_200_206[0] = "OK"; 
	this->_200_206[1] = "Created"; 
	this->_200_206[2] = "Accepted"; 
	this->_200_206[3] = "Non-Authoritative Information"; 
	this->_200_206[4] = "No Content"; 
	this->_200_206[5] = "Reset Content"; 
	this->_200_206[6] = "Partial Content";

	this->_300_307[0] = "Multiple Choices";
	this->_300_307[1] = "Moved Permanently";
	this->_300_307[2] = "Found";
	this->_300_307[3] = "See Other";
	this->_300_307[4] = "Not Modified";
	this->_300_307[5] = "Use Proxy";
	this->_300_307[6] = "";//unused
	this->_300_307[7] = "Temporary Redirect";
	
	this->_400_417[0] = "Bad Request";
	this->_400_417[1] = "Unauthorized";
	this->_400_417[2] = "Payment Required";
	this->_400_417[3] = "Forbidden";
	this->_400_417[4] = "Not Found";
	this->_400_417[5] = "Method Not Allowed";
	this->_400_417[6] = "Not Acceptable";
	this->_400_417[7] = "Proxy Authentication Required";
	this->_400_417[8] = "Request Time-out";
	this->_400_417[9] = "Conflict";
	this->_400_417[10] = "Gone";
	this->_400_417[11] = "Length Required";
	this->_400_417[12] = "Precondition Failed";
	this->_400_417[13] = "Request Entity Too Large";
	this->_400_417[14] = "Request-URI Too Large";
	this->_400_417[15] = "Unsupported Media Type";
	this->_400_417[16] = "Requested range not satisfiable";
	this->_400_417[17] = "Expectation Failed";

	this->_500_505[0] = "Internal Server Error";
	this->_500_505[1] = "Not Implemented";
	this->_500_505[2] = "Bad Gateway";
	this->_500_505[3] = "Service Unavailable";
	this->_500_505[4] = "Gateway Time-out";
	this->_500_505[5] = "HTTP Version not supported";
}

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

inline const std::string &ExceptionMessageFactory::getExceptionMessage(int statusCode) const
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
