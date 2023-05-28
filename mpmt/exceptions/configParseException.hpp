#ifndef CONFIGPARSEEXCEPTION
# define CONFIGPARSEEXCEPTION

#include <exception>

class configParseException : public std::exception
{
	public:
		const char *what() const throw()
		{
			return "Config file Parsing Error";
		}
};

#endif
