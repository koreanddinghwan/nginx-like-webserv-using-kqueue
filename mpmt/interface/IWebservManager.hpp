#ifndef IWEBSERVMANAGER_HPP
# define IWEBSERVMANAGER_HPP

class IWebservManager
{
public:
	virtual void initConfig(int ac, char **av) const = 0;
	virtual void initLoop() = 0;
};


#endif
