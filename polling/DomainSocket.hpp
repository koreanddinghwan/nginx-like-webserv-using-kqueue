#ifndef DOMAINSOCKET_HPP
# define DOMAINSOCKET_HPP

#include "./ISocket.hpp"
#include <netinet/in.h>
#include <sys/_types/_sa_family_t.h>
#include <sys/socket.h>

/**
 * @brief unix domain socket 생성용 클래
 * IPC용으로 사용된다, AF_UNIX, SOCK_STREAM 고정
 */
class DomainSocket : public ISocket 
{
private:
	const int InetLayerProtocol = AF_UNIX;
	const int TpLayerProtocol = SOCK_STREAM;
	int IpcSkFd;
	char *sun_path;
	T_SocketOptionVec SockOptVec;
	/**
	 * @brief compile time에 결정해야하는 구조체
	 */
	struct sockaddr_un {
		sa_family_t sun_family;
		char sun_path[108];
	} domain_unix_addr;

	/**
	 * @brief make socket instance 
	 *
	 * @param inetLayerProtocol: 인터넷 계층 프로토콜
	 * @param tpLayerProtocol: 전송계층 프로토콜
	 * @param sockOptVec: setsockopt 설정 option 벡터.
	 */
	DomainSocket(T_SocketOptionVec sockOptVec, const char *sockPath) :
		SockOptVec(sockOptVec)
	{
		strcpy(this->domain_unix_addr.sun_path, sockPath);
		this->openSocket();
		this->setSockOptions();
		this->setSocketAddress();
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
		this->IpcSkFd = socket(this->InetLayerProtocol, this->TpLayerProtocol, 0);
		if (this->IpcSkFd < 0)
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
			if (setsockopt(this->IpcSkFd, it->first, it->second.first, &(it->second.second), sizeof(it->second.second)) < 0)
				throw std::runtime_error("ERROR: setsockopt");
		}
	}

	/**
	 * @brief set address of domain socket
	 */
	void setSocketAddress() 
	{
		memset(&this->domain_unix_addr, 0, sizeof(this->domain_unix_addr));
		this->domain_unix_addr.sun_family = AF_UNIX;
	}




	/**
	 * @brief naming the unnamed Ipc socket
	 *
	 * @throw std::runtime_error
	 */
	void sockBind() throw (std::runtime_error) 
	{
		if (::bind(this->IpcSkFd, (struct sockaddr*)&domain_unix_addr, sizeof(domain_unix_addr)) < 0)
			throw std::runtime_error("ERROR: bind");
	}

	/**
	 * @brief listen socket in IpcskFd
	 *
	 * @throw std::runtime_error
	 */
	void sockListen() throw (std::runtime_error) 
	{
		if (::listen(this->IpcSkFd, SOMAXCONN) < 0)
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
		return this->IpcSkFd;
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
