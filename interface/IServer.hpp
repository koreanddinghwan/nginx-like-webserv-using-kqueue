#ifndef ISERVER_HPP
# define ISERVER_HPP

#include "IHandler.hpp"
#include <stdexcept>

class IServer
{
public:
	IHandler *requestHandler;
	IHandler *responseHandler;
	~IServer() {}
	virtual void init() throw (std::runtime_error) {}
};

#endif
