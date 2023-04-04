#ifndef ISOCKET_HPP
# define ISOCKET_HPP

#include <vector>
#include "../../interface/IModule.hpp"

template <class initArgType, class runArgType>
class ISocket : public IModule<initArgType, runArgType>
{

	public:
	typedef typename std::vector<std::pair<int, std::pair<int, int> > > T_SocketOptionVec;
	/**
	 * @param first : level
	 * @param second->first: option name
	 * @param second->second: option value
	 **/
	virtual int getSkFd() const = 0;
	virtual int getInetLayerProtocol() const = 0;
	virtual int getTcpLayerProtocol() const = 0;
	virtual T_SocketOptionVec getSockOptVec() const = 0; 
};

#endif
