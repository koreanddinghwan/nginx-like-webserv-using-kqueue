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
	virtual int addModule(IModule *m) throw (std::runtime_error) = 0;
	virtual void runServer() throw (std::runtime_error) = 0;
	virtual ~IServer() {}
};

#endif
