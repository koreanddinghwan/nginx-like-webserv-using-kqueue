#ifndef ISERVER_HPP
# define ISERVER_HPP

#include "IModule.hpp"
#include <stdexcept>

/**
 * @brief 모듈을 받아서 관리하는 클래스
 */
class IServer
{
public:
	virtual void setConfigModule(IModule *m) = 0;
	virtual void setSocketModule(IModule *m) throw (std::runtime_error) = 0;


	#ifdef MULTIPROCESS
	void setIpcSocket(IModule *m)  = 0;
	#endif


	virtual void getConfigModule() = 0;
	virtual void runServer() throw (std::runtime_error) = 0;
	virtual ~IServer() {}
};

#endif
