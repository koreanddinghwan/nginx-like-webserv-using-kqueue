#ifndef SERVER_HPP
# define SERVER_HPP

#include "interface/IManager.hpp"
class Server: public IManager<class moduleInitArgType, class moduleRunArgType>

#endif
