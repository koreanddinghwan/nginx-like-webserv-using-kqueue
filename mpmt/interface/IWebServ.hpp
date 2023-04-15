#ifndef IWEBSERV_HPP
# define IWEBSERV_HPP


class IWebServ
{
public:
	virtual void init(int ac, char **av) = 0;
	virtual void run() = 0;
};


#endif
