#ifndef INTERNET_ENUM_HPP
# define INTERNET_ENUM_HPP

#include <sys/socket.h>
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

# endif
