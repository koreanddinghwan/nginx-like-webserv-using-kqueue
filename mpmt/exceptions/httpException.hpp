#ifndef HTTPREXCEPTION_HPP
# define HTTPREXCEPTION_HPP

#include <string>
#include <exception>
#include <vector>

/**
 * https://www.rfc-editor.org/rfc/rfc2616.html
 * */
class ExceptionMessageFactory
{
	private:
		const std::vector<const std::string> _100_101 = {
			"Continue", 
			"Switching Protocols"
		};

		const std::vector<const std::string> _200_206 = {
			"OK", 
			"Created", 
			"Accepted", 
			"Non-Authoritative Information", 
			"No Content", 
			"Reset Content", 
			"Partial Content"
		};

		const std::vector<const std::string> _300_307 = {
			"Multiple Choices", 
			"Moved Permanently", 
			"Found", 
			"See Other", 
			"Not Modified", 
			"Use Proxy", 
			"", //unused
			"Temporary Redirect"
		};

		const std::vector<const std::string> _400_417 = {
			"Bad Request", 
			"Unauthorized", 
			"Payment Required", 
			"Forbidden", 
			"Not Found", 
			"Method Not Allowed", 
			"Not Acceptable", 
			"Proxy Authentication Required", 
			"Request Time-out", 
			"Conflict", 
			"Gone", 
			"Length Required", 
			"Precondition Failed", 
			"Request Entity Too Large", 
			"Request-URI Too Large", 
			"Unsupported Media Type", 
			"Requested range not satisfiable", 
			"Expectation Failed"
		};

		const std::vector<const std::string> _500_505 = {
			"Internal Server Error", 
			"Not Implemented", 
			"Bad Gateway", 
			"Service Unavailable", 
			"Gateway Time-out", 
			"HTTP Version not supported"
		};
	public:
		static ExceptionMessageFactory &getInstance();
		const std::string &getExceptionMessage(int statusCode) const;

	private:
		ExceptionMessageFactory();
		~ExceptionMessageFactory();
		ExceptionMessageFactory(const ExceptionMessageFactory &other);
		ExceptionMessageFactory &operator=(const ExceptionMessageFactory &other);
};

class HttpException : public std::exception
{
	private:
		const std::string &message;
		int statusCode;

	public:
	HttpException(int statusCode);

    const char* what() const throw();

    int getStatusCode() const;

};

#endif
