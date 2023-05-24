#ifndef IHANDLER_HPP
# define IHANDLER_HPP

class IHandler
{
public:
	virtual void *handle(void *data) = 0;
};

#endif
