#ifndef ISERVER_HPP
# define ISERVER_HPP

#include "./ISocket.hpp"
#include <vector>

class IServer {
public:
	virtual void setServerConf() = 0;
	virtual void setServerSockets() = 0;
	virtual void makeWorkerProcess() = 0;

	virtual ~IServer() {}
};


#endif
