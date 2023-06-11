#ifndef IWEBSERV_HPP
# define IWEBSERV_HPP

#include "../exceptions/configParseException.hpp"
#include <stdexcept>

class IWebServ
{
public:
	virtual void run(int ac, char **av) const = 0;
};


#endif
