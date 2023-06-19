#ifndef WS_HTTPINDEXMODULE_HPP
# define WS_HTTPINDEXMODULE_HPP

#include "../eventLoop/Event.hpp"
#include <sys/stat.h>
#include <sys/fcntl.h>

class ws_HttpIndexModule
{
public:
	static bool processEvent(Event *e);
};


#endif
