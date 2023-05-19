#ifndef EVENT_HPP
#define EVENT_HPP

#include "../http/HttpReqHandler.hpp"
#include <netinet/in.h>
#include "../../interface/IServer.hpp"

/* Event Type: */

/* 1. Read (FILT)*/ 
/* 	- Server Socket Read */
/* 	- Client Socket Read */
/* 	- Pipe Read */
/* 	- File Read */


/* 2. Write (FILT)*/
/* 	- Client Socket Write */
/* 	- Pipe Write */
/* 	- File Write */

/* 3. Error */



typedef enum s_ServerType {
	HTTP_SERVER,
	HTTPS_SERVER,
	FTP_SERVER,
	SMTP_SERVER,
	POP3_SERVER,
	IMAP_SERVER,
	TELNET_SERVER,
	SSH_SERVER
} t_ServerType;

typedef enum s_FdType {
	SERVER_SOCKET_FD,
	CLIENT_SOCKET_FD,
	PIPE_FD,
	FILE_FD
} t_FdType;

typedef struct s_FdInfo {
	int			fd;
	t_FdType	fdType;
} t_FdInfo;

typedef struct s_SocketInfo
{
	struct sockaddr_in socket_addr;
	int opt;
	int keep_alive;
	int nagle_off;
} t_SocketInfo;

class Event {
private:
	/* 이 이벤트는 어느 서버에서 처리될 fd인가? */
	t_ServerType	serverType;

	/* socket생성시의 정보 */
	t_SocketInfo	socketInfo;

	/* 이 이벤트의 fd정보 */
	/* maybe....some...,,ummm, kevent? ...no socket info!*/
	t_FdInfo	fdInfo;

	/* 이벤트를 처리할 핸들러*/
	/* umm,,, request info*/
	/* @obsovo */
	IHandler*	requestHandler;

public:
	Event(t_ServerType t);
	~Event();

public:
	void setFdType(t_FdType *t);
	void setFdInfo(t_FdType t);
	void setSocketInfo(t_SocketInfo *t);
	void setRequestHandler();


private:
	Event();
	const Event& operator=(const Event &e);
	Event(Event &e);
};


#endif
