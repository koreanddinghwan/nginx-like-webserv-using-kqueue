#ifndef HTTPREQHANDLER_HPP
# define HTTPREQHANDLER_HPP

#include "./HttpRequestInfo.hpp"
#include "../../interface/IHandler.hpp"

/**
 * @brief static class
 */
class HttpreqHandler : public IHandler
{
private:
	HttpreqHandler() {}
	~HttpreqHandler() {}

public:
	static void *handler(char *buffer) 
	{
		httpRequestInfo *rtn = new httpRequestInfo();
		/*parsing!!!!!!!!!!!!!*/
		rtn->others = buffer;
		return rtn;
	}
};

#endif
