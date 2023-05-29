#include "configParseException.hpp"

const char *configParseException::what() const throw()
{
	return "ConfigParseException";
}
