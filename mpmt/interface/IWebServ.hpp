#ifndef IWEBSERV_HPP
# define IWEBSERV_HPP

#include "../exceptions/configParseException.hpp"
#include <stdexcept>

class IWebServ
{
public:
	virtual void init(int ac, char **av) const throw(std::runtime_error, configParseException) = 0;
	virtual void run() const = 0;
};


#endif
