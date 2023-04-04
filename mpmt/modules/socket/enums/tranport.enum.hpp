#ifndef NETWORK_ENUM_HPP
# define NETWORK_ENUM_HPP

/**
 * @brief define socket arguments enum by human-readable
 * SOCKET socket(int af,int type,int protocol);
 */

#include <netinet/in.h>
#include <sys/socket.h>

/**
 * @brief af in SOCKET socket(int af,int type,int protocol);
 */
enum ipLayer {
	/**
	 * @brief ipc socket
	 */
	IPCSOCKET = AF_UNIX,
	/**
	 * @brief ipv4
	 */
	IPV4 = AF_INET,
	/**
	 * @brief ipv6
	 */
	IPV6 = AF_INET6
};

/**
 * @brief type in SOCKET socket(int af,int type,int protocol);
 */
enum tcpLayerType {
	/**
	 * @brief tcp type socket
	 */
	TCPSOCKETTYPE = SOCK_STREAM,

	/**
	 * @brief udp type socket
	 */
	UDPSOCKETTYPE = SOCK_DGRAM,
	/**
	 * @brief raw type socket
	 */
	RAWTYPE = SOCK_RAW
};

/**
 * @brief protocol in SOCKET socket(int af,int type,int protocol);
 */
enum tcpLayerProtocol {

	/**
	 * @brief tcp protocol
	 */
	TCP = IPPROTO_TCP,

	/**
	 * @brief udp protocol
	 */
	UDP = IPPROTO_UDP,

	/**
	 * @brief raw protocol
	 */
	RAW = IPPROTO_RAW
};

#endif
