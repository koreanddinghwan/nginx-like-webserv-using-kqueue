#ifndef REQUESTHANDLER_HPP
# define REQUESTHANDLER_HPP

#include "./HttpRequestInfo.hpp"
#include "../../interface/IHandler.hpp"

class requestHandler : public IHandler
{

public:
	requestHandler() {}
	~requestHandler() {}

	httpRequestInfo *handler(char *buffer) 
	{
		httpRequestInfo *rtn = new httpRequestInfo();
		/*parsing!!!!!!!!!!!!!*/
		rtn->others = buffer;
		return rtn;
	}
};

#endif
