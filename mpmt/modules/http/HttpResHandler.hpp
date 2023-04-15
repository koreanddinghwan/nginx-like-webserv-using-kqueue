#ifndef HTTPRESHANDLER_HPP
# define HTTPRESHANDLER_HPP

#include "HttpRequestInfo.hpp"
#include "../../interface/IHandler.hpp"

/**
 * @brief static class
 */
class HttpresHandler : public IHandler
{
private:
	HttpresHandler() {}
	~HttpresHandler() {}

public:
	static void *handler(void *reqInfo) {

		//do something with request information
	}
};

#endif
