#ifndef HTTPREQHANDLER_HPP
# define HTTPREQHANDLER_HPP

#include "./HttpRequestInfo.hpp"
#include "../../interface/IHandler.hpp"

class HttpreqHandler : public IHandler
{
private:
	httpRequestInfo *info;

public:
	HttpreqHandler();
	~HttpreqHandler();

public:
	void *handle(void *data);
};

#endif
