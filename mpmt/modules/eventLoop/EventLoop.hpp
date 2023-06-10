#ifndef EVENTLOOP_HPP
# define EVENTLOOP_HPP

#include "../../factory/MethodFactory.hpp"
#include "../../exceptions/httpException.hpp"
#include <cstdlib>
#include <exception>
#include <sys/_types/_socklen_t.h>
#include <sys/_types/_ssize_t.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/event.h>
#include <stdexcept>
#include "../config/Config.hpp"
#include "../../interface/IBlock.hpp"
#include "../http/HttpServer.hpp"
#include "Event.hpp"
#include "../http/responseHandler.hpp"
#include "../http/HttpreqHandler.hpp"
#include "../http/ws_HttpIndexModule.hpp"
#include "../http/ws_HttpAutoIndexModule.hpp"
#include "../http/ws_HttpUploadModule.hpp"
/**
 * @brief singleton eventloop
 */
class EventLoop
{
public:
	/**
	 * @brief get instance of eventloop
	 *
	 * @return eventloop instance
	 */
	static EventLoop& getInstance();

	/**
	 * @brief init eventloop
	 * 핵심모듈
	 */
	void initEventLoop();

	char pipeBuffer[HTTPBUFFER_SIZE];

private:
	int kq_fd;

	struct kevent dummyEvent;

	struct sockaddr_in dummy_addr;

private:
	/**
	 * @brief initialize events that each server has
	 */
	void initServerEvents();


	EventLoop();
	~EventLoop();
	void printCurrentData();


	void readCallback(struct kevent *e);
	void writeCallback(struct kevent *e);

	void e_serverSocketReadCallback(struct kevent *e, Event *e_udata);
	void e_clientSocketReadCallback(struct kevent *e, Event *e_udata);
	void e_pipeReadCallback(struct kevent *e, Event *e_udata);
	void e_fileReadCallback(struct kevent *e, Event *e_udata);


	void e_clientSocketWriteCallback(struct kevent *e, Event *e_udata);
	void e_pipeWriteCallback(struct kevent *e, Event *e_udata);
	void e_fileWriteCallback(struct kevent *e, Event *e_udata);

	/**
	 * kqueue에 event를 등록합니다.
	 * */
	void registerClientSocketReadEvent(Event *e);
	void registerPipeReadEvent(Event *e);
	void registerFileReadEvent(Event *e);
	void registerClientSocketWriteEvent(Event *e);
	void registerPipeWriteEvent(Event *e);
	void registerFileWriteEvent(Event *e);


	void unregisterClientSocketReadEvent(Event *e);
	void unregisterPipeReadEvent(Event *e);
	void unregisterFileReadEvent(Event *e);
	void unregisterClientSocketWriteEvent(Event *e);
	void unregisterPipeWriteEvent(Event *e);
	void unregisterFileWriteEvent(Event *e);

	void errorCallback(Event *e);
	/**
	 * response setter
	 * */
	void setHttpResponse(Event *e);
	void ws_internalRedir(Event *e);

	bool processCgi(Event *e);
	
	void ws_method_GET(Event *e);
	void ws_method_POST(Event *e);
	void ws_method_DELETE(Event *e);
};


#endif
