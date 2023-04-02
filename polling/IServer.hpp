#ifndef ISERVER_HPP
# define ISERVER_HPP

class IServer {
public:
	virtual void m_setSocket() = 0;
	virtual void m_listen() = 0;
	virtual ~IServer() {}
};


#endif
