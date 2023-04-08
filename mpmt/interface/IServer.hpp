#ifndef ISERVER_HPP
# define ISERVER_HPP

#include <stdexcept>
class IServer
{
public:
	~IServer() {}
	virtual void run() throw (std::runtime_error) {}
};

#endif
