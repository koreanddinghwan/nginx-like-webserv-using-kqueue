#ifndef ISOCKET_HPP
# define ISOCKET_HPP

#include <vector>

class ISocket {

	public:
	/**
	 * @param first : level
	 * @param second->first: option name
	 * @param second->second: option value
	 **/
	typedef typename std::vector<std::pair<int, std::pair<int, int> > > T_SocketOptionVec;
	typedef typename T_SocketOptionVec::iterator T_SocketOptIterator;

	virtual int getSkFd() const = 0;
	virtual int getInetLayerProtocol() const = 0;
	virtual int getTcpLayerProtocol() const = 0;
	virtual T_SocketOptionVec getSockOptVec() const = 0; 

	virtual void openSocket() throw (std::runtime_error) = 0;
	virtual void setSockOptions() throw (std::runtime_error) = 0;
};

#endif
