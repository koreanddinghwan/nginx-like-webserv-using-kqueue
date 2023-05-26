#ifndef HTTPRESHANDLER_HPP
# define HTTPRESHANDLER_HPP

#include "HttpRequestInfo.hpp"
#include "../../interface/IHandler.hpp"

/**
 * @brief static class
 */
class HttpResHandler : public IHandler
{
private:
	HttpResHandler();
	~HttpResHandler();

public:
	void *handler(void *data); 
};

#endif
