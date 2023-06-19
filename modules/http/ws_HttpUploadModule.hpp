#ifndef WS_HTTPUPLOADMODULE_HPP
# define WS_HTTPUPLOADMODULE_HPP

#include "HttpRequestHandler.hpp"
#include "../eventLoop/Event.hpp"
#include <sys/stat.h>
#include <sys/fcntl.h>

class ws_HttpUploadModule
{
public:
	static bool processEvent(Event *e);
};


#endif
