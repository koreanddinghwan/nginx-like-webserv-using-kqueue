#ifndef RESPONSEHANDLER_HPP
# define RESPONSEHANDLER_HPP

#include "HttpRequestInfo.hpp"
#include "../../interface/IHandler.hpp"

class responseHandler : public IHandler
{
public:
	responseHandler() {}
	~responseHandler() {}

	void *handler(void *reqInfo) {

	}
};

#endif
