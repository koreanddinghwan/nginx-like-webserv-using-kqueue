#ifndef WS_HTTPAUTOINDEXMODULE_HPP
# define WS_HTTPAUTOINDEXMODULE_HPP

#include "../eventLoop/Event.hpp"
#include <dirent.h>
#include <stdlib.h>
#include <string>
#include <sys/stat.h>
#include <vector>
#include "HttpRequestHandler.hpp"
#include "HttpResponseHandler.hpp"
#include <sys/stat.h>

/**
 * @TODO
 * */
class ws_HttpAutoIndexModule
{
public:
	static bool processEvent(Event *e);
};


#endif
