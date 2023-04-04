#ifndef SOCKET_HPP
# define SOCKET_HPP

#include "./ISocket.hpp"
#include <netinet/in.h>
#include <stdexcept>
#include <sys/_endian.h>
#include <vector>
#include <sys/socket.h>
#include <utility>

template <class initArgType, class runArgType>
class Socket : public ISocket<initArgType, runArgType>
{
public:
	typedef typename std::vector<std::pair<int, std::pair<int, int> > > T_SocketOptionVec;
	typedef typename T_SocketOptionVec::iterator T_SocketOptIterator;

private:

	int SkFd;
	int Port;
	int InetLayerProtocol;
	int TpLayerProtocol;
	T_SocketOptionVec SockOptVec;
	struct sockaddr_in server_addr;

public:
	/**
	 * @brief make socket instance 
	 *
	 * @param inetLayerProtocol: 인터넷 계층 프로토콜
	 * @param tpLayerProtocol: 전송계층 프로토콜
	 * @param sockOptVec: setsockopt 설정 option 벡터.
	 */
	Socket(int inetLayerProtocol, int tpLayerProtocol, T_SocketOptionVec sockOptVec, int port) :
		InetLayerProtocol(inetLayerProtocol),
		TpLayerProtocol(tpLayerProtocol),
		SockOptVec(sockOptVec),
		Port(port)
	{}

	void init(initArgType args) throw (std::runtime_error)
	{
		this->openSocket();
		this->setSockOptions();
		this->setSocketAddress();
	}

	void run(runArgType args) throw (std::runtime_error)
	{
		this->sockBind();
		this->sockListen();
	}


private:
	/**
	 * @brief open socket
	 *
	 * @throw std::runtime_error
	 */
	void openSocket() throw (std::runtime_error)
	{
		this->SkFd = socket(this->InetLayerProtocol, this->TpLayerProtocol, 0);
		if (this->SkFd < 0)
			throw std::runtime_error("ERROR: socket");

	}

	/**
	 * @brief setSocket's option
	 *
	 * @throw std::runtime_error
	 */
	void setSockOptions() throw (std::runtime_error) {
		//set socket option
		for (T_SocketOptIterator it = SockOptVec.begin(); it != SockOptVec.end(); it++)
		{
			if (setsockopt(this->SkFd, it->first, it->second.first, &(it->second.second), sizeof(it->second.second)) < 0)
				throw std::runtime_error("ERROR: setsockopt");
		}
	}

	/**
	 * @brief set address of socket
	 */
	void setSocketAddress() 
	{
		memset(&this->server_addr, 0, sizeof(server_addr));
		server_addr.sin_family = this->InetLayerProtocol;
		server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
		server_addr.sin_port = htons(this->Port);
	}

	/**
	 * @brief naming the unnamed socket
	 *
	 * @throw std::runtime_error
	 */
	void sockBind() throw (std::runtime_error) 
	{
		if (::bind(this->SkFd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
			throw std::runtime_error("ERROR: bind");
	}

	/**
	 * @brief listen socket in skFd
	 *
	 * @throw std::runtime_error
	 */
	void sockListen() throw (std::runtime_error) 
	{
		if (::listen(this->SkFd, SOMAXCONN) < 0)
			throw std::runtime_error("ERROR: listen");
	}

	public:
	/**
	 * @brief get socket's fd
	 *
	 * @return Socket instance's socket fd
	 */
	int getSkFd() const 
	{
		return this->SkFd;
	}

	/**
	 * @brief get internet layer protocol
	 *
	 * @return AF_INET(ipv4)
	 */
	int getInetLayerProtocol() const
	{
		return this->InetLayerProtocol;
	}


	/**
	 * @brief get tcp layer protocol
	 *
	 * @return SOCK_STREAM
	 */
	int getTcpLayerProtocol() const
	{
		return this->TpLayerProtocol;
	}

	/**
	 * @brief get socket option vector
	 *
	 * @return T_SocketOptionVec vec
	 */
	T_SocketOptionVec getSockOptVec() const 
	{
		return this->SockOptVec;
	}

};

#endif
