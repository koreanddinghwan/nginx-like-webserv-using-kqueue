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
		std::vector<std::string> _100_101;
		std::vector<std::string> _200_206;
		std::vector<std::string> _300_307;
		std::vector<std::string> _400_417;
		std::vector<std::string> _500_505;

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
