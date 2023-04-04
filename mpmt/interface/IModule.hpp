#ifndef IMODULE_HPP
# define IMODULE_HPP

#include <stdexcept>

template<class initArgType, class runArgType>
class IModule
{
public:
	virtual void init(initArgType args) throw (std::runtime_error) = 0;
	virtual void run(runArgType args) throw (std::runtime_error) = 0;
	virtual ~IModule() {}
};

#endif
