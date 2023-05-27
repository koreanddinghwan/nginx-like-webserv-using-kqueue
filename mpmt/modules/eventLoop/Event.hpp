#ifndef EVENT_HPP
#define EVENT_HPP

#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/event.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include "../config/Config.hpp"
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
	E_SERVER_SOCKET = 0,
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
public:
		typedef std::vector<HttpLocationData *> t_locationData;


private:
	/* 이 이벤트는 어느 서버에서 처리될 fd인가? */
	t_ServerType	serverType;

	/* socket생성시의 정보 */
	t_SocketInfo	socketInfo;

	/* 이 이벤트의 fd정보 */
	/* maybe....some...,,ummm, kevent? ...no socket info!*/
	/**
	 * @brief 이벤트의 fd정보
	 * @todo pipe를 처리하면, 그 파이프의 out pipe를 처리할 fd는 어디에?
	 * fd의 종류별로 처리할 일이 다름.
	 * file open fd의 read 준비가 끝났을때, open한 파일의 read이후, 어디로 처리?
	 * 종류별로 모두 들고있는게 나을 것 같음.
	 */
	int	server_socket_fd;
	int client_socket_fd;
	int pipe_fd;
	int file_fd;

	t_EventType	eventInfo;

	/* 이벤트를 처리할 핸들러*/
	/* umm,,, request info*/
	/* @obsovo */
	IHandler*	requestHandler;
	IHandler*	responseHandler;

	/**
	 * @brief 현재 이벤트의 serverdata
	 * */
	std::vector<HttpServerData *> serverData;

	/**
	 * @brief todo: 생성자에서 초기화
	 */
	HttpServerData *defaultServerData;


public:
	Event(t_ServerType t);
	/* Event(Event &e); */
	~Event();

private:
	/**
	 * private setter
	 * */
	void setServerType(t_ServerType t);
	void setSocketInfo(t_SocketInfo t);
	void setServerFd(int t);
	void setClientFd(int t);
	void setPipeFd(int t);
	void setFileFd(int t);
	void setEventType(t_EventType t);

public:
	void setRequestHandler(IHandler* t);
	void setResponseHandler(IHandler* t);
	void setServerDataByPort(int port);
	void setServerData(std::vector<HttpServerData *> *t);

public:
	/**
	 * getter
	 * */
	t_ServerType& getServerType();
	t_SocketInfo& getSocketInfo();
	int& getServerFd();
	int& getClientFd();
	int& getPipeFd();
	int& getFileFd();

	t_EventType& getEventType();
	IHandler* getRequestHandler();
	IHandler* getResponseHandler();
	std::vector<HttpServerData *> *getServerData();
	HttpServerData *getDefaultServerData();

	static Event* createNewServerSocketEvent(int port);
	static Event* createNewClientSocketEvent(Event *e);

private:
	Event(Event &e);
	Event();
	const Event& operator=(const Event &e);
};


#endif
