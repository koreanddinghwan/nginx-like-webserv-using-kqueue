#ifndef IWEBSERV_HPP
# define IWEBSERV_HPP


class IWebServ
{
public:
	virtual void init(int ac, char **av) const = 0;
	virtual void run() const = 0;
};


#endif
