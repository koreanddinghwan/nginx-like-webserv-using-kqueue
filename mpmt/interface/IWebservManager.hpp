#ifndef IWEBSERVMANAGER_HPP
# define IWEBSERVMANAGER_HPP

class IWebservManager
{
public:
	virtual void initServers() = 0;
	virtual void initLoop() = 0;
};


#endif
