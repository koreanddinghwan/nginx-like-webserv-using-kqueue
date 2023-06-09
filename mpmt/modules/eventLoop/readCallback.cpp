#include "Event.hpp"
#include "EventLoop.hpp"
#include <exception>
#include <sys/event.h>

void EventLoop::readCallback(struct kevent *e)
{
	std::cout << "\033[32m"; 
	std::cout<<"Read callback"<<std::endl;

	//set event
	Event *e_udata = static_cast<Event *>(e->udata);
	switch (e_udata->getEventType()){
		//check us server socket
		case E_SERVER_SOCKET:
			e_serverSocketReadCallback(e, e_udata);
			break;
		//check is client socket read
		case E_CLIENT_SOCKET:
			e_clientSocketReadCallback(e, e_udata);
			break;
		//check is pipe read
		case E_PIPE:
			e_pipeReadCallback(e, e_udata);
			break;
		//check is file read
		case E_FILE:
			e_fileReadCallback(e, e_udata);
			break;
		case E_TMP:
			e_tmpFileReadCallback(e, e_udata);
			break;

		default:
			std::cout<<"unknown event type"<<std::endl;
			break;
	}
}

/**
 * @brief serverSocket의 read event callback.
 *
 * server socket에서 발생한 read event에 대해 
 * client socket을 생성하고, client socket에 대한 read event를 등록한다.
 * @param e
 */
void EventLoop::e_serverSocketReadCallback(struct kevent *e, Event *e_udata)
{
	//we need to verify http
	if (e_udata->getServerType() == HTTP_SERVER)
	{
		//create client socket
		//모든 pipe, file의 이벤트는 client socket에서 부터 시작한다.
		Event *new_udata = Event::createNewClientSocketEvent(e_udata);

		//handler 객체 설정
		new_udata->setRequestHandler(new HttpreqHandler(new_udata));
		new_udata->setResponseHandler(new responseHandler(new_udata));


		//kqueue에 event 등록
		registerClientSocketReadEvent(new_udata);
	}
}

void EventLoop::e_clientSocketReadCallback(struct kevent *e, Event *e_udata)
{
	std::cout << "\033[34m";
	std::cout<<"client socket read callback"<<std::endl;

	//Http Server인 소켓에서 연결된 client_fd에 대한 read처리
	if (e_udata->getServerType() == HTTP_SERVER)
	{
		//read from client socket
		int client_fd = e_udata->getClientFd();
		ssize_t read_len = read(client_fd, HttpServer::getInstance().getHttpBuffer(), HTTPBUFFER_SIZE - 1);
	
		std::cerr<<"read len : "<<read_len<<std::endl;
		if (read_len == -1)
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK)
				return;
			else if (errno == ECONNRESET)
			{
				std::cerr<<"ECONNRESET"<<std::endl;

				//remove event
				unregisterClientSocketReadEvent(e_udata);
				//client socket close
				close(client_fd);
				//client socket event delete
				delete e_udata;
				return;
			}
			else
				//관련 exception 처리 필요
				//event 삭제?
				throw std::runtime_error("Failed to read from client socket, unknown err\n");
		}
		else if (read_len == 0)
		{
			unregisterClientSocketReadEvent(e_udata);
			//client socket close
			close(client_fd);
			//client socket event delete
			delete e_udata;
			return;
		}
		else
		{
			HttpServer::getInstance().getHttpBuffer()[read_len] = '\0';
			e_udata->readByte = read_len;
			HttpreqHandler *reqHandler = static_cast<HttpreqHandler *>(e_udata->getRequestHandler());
			//handle request
			try {
				/* std::cout<<"use handle"<<std::endl; */
				reqHandler->handle(e_udata);
				/* std::cout<<"use handle end"<<std::endl; */
			} catch (std::exception &exception) {
				std::cerr<<"handle exception"<<std::endl;
				errorCallback(e_udata);
				return;
			}

			/**
			 * pending state => client로부터 data를 더 받아야하는 상태
			 * */
			std::cerr<<"is pending : "<<reqHandler->getIsPending()<<std::endl;
			std::cerr<<"is headerpending : "<<reqHandler->isHeaderPending()<<std::endl;
			std::cerr<<"is bodypending : "<<reqHandler->isBodyPending()<<std::endl;

			if (reqHandler->getIsPending())
				return ;
			else
			{
				std::cerr<<"set response"<<std::endl;
				/**
				 * initialize internal method and uri
				 * */
				e_udata->internal_method = reqHandler->getRequestInfo().method;
				e_udata->internal_uri = reqHandler->getRequestInfo().path;
				setHttpResponse(e_udata);
			}
		}
	}
}

/**
* Fifos, Pipes
* Returns when there is data to read; data contains the number of bytes available.
* When the last writer disconnects, the filter will set EV_EOF in flags.
* This may be cleared by passing in EV_CLEAR, at which point the filter will
* resume waiting for data to become available before returning.
*
* @ read from cgi
**/
void EventLoop::e_pipeReadCallback(struct kevent *e, Event *e_udata)
{
	std::cout<<"pipe Read"<<std::endl;
	if (e_udata->getServerType() == HTTP_SERVER)
	{
		//read from pipe
		ssize_t read_len = read(e_udata->CtoPPipe[0], EventLoop::getInstance().pipeBuffer, PIPEBUFFERSIZE - 1);

		if (read_len == -1)
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK)
				return;
			else
				//관련 exception 처리 필요
				//event 삭제?
			{
				std::cerr<<"pipe read error. read len is -1, errno: "<<errno<<std::endl;
				e_udata->setStatusCode(500);
				unregisterPipeReadEvent(e_udata);
				unlink(e_udata->tmpOutFileName.c_str());
				registerClientSocketWriteEvent(e_udata);
				return;
			}
		}
		else if (read_len == 0 && e->flags & EV_EOF)
		{
			/**
			 * EV_EOF to pipe : cgi process end
			 * @ref man 2 kevent
			 * Fifos, Pipes
			 * Returns when there is data to read; data contains the number of bytes available.
			 * When the last writer disconnects, the filter will set EV_EOF in flags.
			 * */
			std::cerr<<"readlen = 0, unregister pipe and register write event"<<"total pipe read: "<<e_udata->fileReadByte <<std::endl;
			unregisterPipeReadEvent(e_udata);
			unlink(e_udata->tmpOutFileName.c_str());
			registerClientSocketWriteEvent(e_udata);
			return;
		}
		else
		{
			EventLoop::getInstance().pipeBuffer[read_len] = '\0';
			e_udata->fileReadByte += read_len;
			static_cast<responseHandler *>(e_udata->getResponseHandler())->setResBody(EventLoop::getInstance().pipeBuffer);
		}
	}
}

void EventLoop::e_fileReadCallback(struct kevent *e, Event *e_udata)
{
	std::cout<<"fileRead"<<std::endl;

	if (e_udata->getServerType() == HTTP_SERVER)
	{
		//read from file
		ssize_t read_len = read(e_udata->file_fd, HttpServer::getInstance().getHttpBuffer(), HTTPBUFFER_SIZE - 1);

		if (read_len == -1)
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK)
				return ;
			else
				throw std::runtime_error("Failed to read from file, unknown err\n");
		}
		else if (read_len == 0)
		{
			unregisterFileReadEvent(e_udata);
			registerClientSocketWriteEvent(e_udata);
			return ;
		}
		else
			{
			HttpServer::getInstance().getHttpBuffer()[read_len] = '\0';
			static_cast<responseHandler *>(e_udata->getResponseHandler())->setResBody(HttpServer::getInstance().getHttpBuffer());
			e_udata->fileReadByte += read_len;

			if (e_udata->fileReadByte == e_udata->statBuf.st_size)
			{
				unregisterFileReadEvent(e_udata);
				registerClientSocketWriteEvent(e_udata);
				return ;
			}
		}
	}
}

void EventLoop::e_tmpFileReadCallback(struct kevent *e, Event *e_udata)
{
	std::cerr<<"TmpfileVNODE"<<std::endl;
	if (e_udata->getServerType() == HTTP_SERVER)
	{
		HttpreqHandler *reqHandler = static_cast<HttpreqHandler *>(e_udata->getRequestHandler());
		//read from file
		ssize_t read_len = 0;

		read_len = read(e_udata->tmpInFile, HttpServer::getInstance().getHttpBuffer(), HTTPBUFFER_SIZE - 1);
		stat(e_udata->tmpInFileName.c_str(), &e_udata->statBuf);
		std::cerr<<"read len: "<<read_len<<std::endl;
		std::cerr<<"event data: "<<e->data<<std::endl;
		std::cerr<<"event flag: "<<e->flags<<std::endl;
		std::cerr<<"event ff: "<<e->fflags<<std::endl;
		std::cerr<<"file byte:" <<e_udata->fileReadByte<<std::endl;
		std::cerr<<"file size: "<<e_udata->statBuf.st_size<<std::endl;
		if (read_len == -1)
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK)
				return ;
			else
			{
				std::cerr<<"tmpfile read error. read len is -1, errno: "<<errno<<std::endl;
				e_udata->setStatusCode(500);
				unregisterTmpFileReadEvent(e_udata);
				registerClientSocketWriteEvent(e_udata);
				return ;
			}
		}
		else if (read_len == 0 && 
				e_udata->fileReadByte == e_udata->statBuf.st_size)
		{
			int status;
			
			//끝났던안끝났던 논블로킹으로 빠르게 리턴
			pid_t terminated_child_pid = waitpid(e_udata->childPid, &status, WNOHANG);
			if (WIFEXITED(status))
			{
				//자식 종료상태면 파일 다 읽은거임
				std::cerr<<"read len is 0, unregister tmpfile read event and register write event"<<std::endl;
				std::cerr<<"child process terminated"<<std::endl;
				std::cerr<<"child status:"<<status<<std::endl;
				e_udata->setStatusCode(200);
				unregisterTmpFileReadEvent(e_udata);
				registerClientSocketWriteEvent(e_udata);
			}
			else
				return;
		}
		else
		{
			HttpServer::getInstance().getHttpBuffer()[read_len] = '\0';
			static_cast<responseHandler *>(e_udata->getResponseHandler())->setResBody(HttpServer::getInstance().getHttpBuffer());
			e_udata->fileReadByte += read_len;
		}
	}
}
