#ifndef ISERVER_HPP
# define ISERVER_HPP

#include "./ISocket.hpp"
#include <vector>

class IServer {
public:
	virtual void setServerSockets() = 0;

	virtual void run() = 0;

	virtual ~IServer() {}
};


#endif
