#ifndef ISERVER_HPP
# define ISERVER_HPP

#include <stdexcept>
class IServer
{
public:
	~IServer() {}
	virtual void initSocket() throw (std::runtime_error) {}
};

#endif
