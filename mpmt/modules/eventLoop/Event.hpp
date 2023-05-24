#ifndef EVENT_HPP
#define EVENT_HPP

#include <cstring>
#include "../http/HttpReqHandler.hpp"
#include <netinet/in.h>
#include "../../interface/IServer.hpp"
#include "../config/data/HttpLocationData.hpp"

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

typedef enum s_EventType {
	E_SERVER_SOCKET,
	E_CLIENT_SOCKET,
	E_PIPE,
	E_FILE
} t_EventType;

typedef struct s_SocketInfo
{
	struct sockaddr_in socket_addr;
	int reUseAddr;
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
	int	fd;

	t_EventType	eventInfo;

	/* 이벤트를 처리할 핸들러*/
	/* umm,,, request info*/
	/* @obsovo */
	IHandler*	requestHandler;

	/**
	 * @brief 현재 이벤트에 대한 location block data
	 */
	std::vector<HttpLocationData *> *configData;


	/**
	 * @brief todo: 생성자에서 초기화
	 */
	std::vector<HttpServerData *> *serverData;


public:
	Event(t_ServerType t);
	Event(Event &e);
	~Event();

public:
	void setServerType(t_ServerType t);
	void setSocketInfo(t_SocketInfo t);
	void setFd(int t);
	void setEventType(t_EventType t);
	void setConfigData(std::vector<HttpLocationData *> *t);

	t_ServerType& getServerType();
	t_SocketInfo& getSocketInfo();
	int& getFd();
	t_EventType& getEventType();
	IHandler* getRequestHandler();
	std::vector<HttpLocationData *> *getConfigData();

private:
	Event();
	const Event& operator=(const Event &e);
};


#endif
