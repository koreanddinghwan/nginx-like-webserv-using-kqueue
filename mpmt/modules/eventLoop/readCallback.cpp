#include "Event.hpp"
#include "EventLoop.hpp"
#include <exception>
#include <sys/event.h>
#include <sys/wait.h>

void EventLoop::readCallback(struct kevent *e)
{
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
		default:
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
		new_udata->setRequestHandler(new HttpreqHandler());
		new_udata->setResponseHandler(new responseHandler(15));

		//kqueue에 event 등록
		registerClientSocketReadEvent(new_udata);
	}
}

void EventLoop::e_clientSocketReadCallback(struct kevent *e, Event *e_udata)
{
	//Http Server인 소켓에서 연결된 client_fd에 대한 read처리
	if (e_udata->getServerType() == HTTP_SERVER)
	{
		//socket의 readfilter-> EOF flag는 client의 disconnect.
		if (e->flags == EV_EOF)
		{
			unregisterClientSocketReadEvent(e_udata);
			//remove event
			//client socket close
			close(e_udata->getClientFd());
			//client socket event delete
			delete e_udata;
		}

		//read from client socket
		int client_fd = e_udata->getClientFd();
		ssize_t read_len = read(client_fd, HttpServer::getInstance().getHttpBuffer(), HTTPBUFFER_SIZE - 1);
  
		if (read_len == -1)
		{
			//remove event
			unregisterClientSocketReadEvent(e_udata);
			//client socket close
			close(client_fd);
			//client socket event delete
			delete e_udata;
			return;
		}
		else if (read_len == 0)
		{
			//remove event
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
				reqHandler->handle(e_udata);
			} catch (std::exception &exception) {
				errorCallback(e_udata);
				return;
			}
			//handle response by request
			/**
			 * pending state => client로부터 data를 더 받아야하는 상태
			 *
			 * read_len : read return 0 when eof
			 * */
			if (reqHandler->getIsPending() && read_len != 0)
			{
				return ;
			}
			else
			{
				/**
				 * set http response
				 * */
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
**/
void EventLoop::e_pipeReadCallback(struct kevent *e, Event *e_udata)
{
	if (e_udata->getServerType() == HTTP_SERVER)
	{
		//read from pipe
		ssize_t read_len = read(e_udata->CtoPPipe[0], EventLoop::getInstance().pipeBuffer, HTTPBUFFER_SIZE - 1);
		if (read_len == -1)
		{
			e_udata->setStatusCode(500);
			unregisterPipeReadEvent(e_udata);
			registerClientSocketWriteEvent(e_udata);
			return;
		}
		else if (read_len == 0)
		{
			//reclaim child process
			//child process가 종료되었을 때, child process의 exit status를 얻어온다.

			int status;
			//cgi 로 fork된 자식프로세스의 상태를 논블로킹으로 확인합니다..
			pid_t pid = waitpid(e_udata->childPid, &status, WNOHANG);
			if (pid == e_udata->childPid)
			{
				//자식 회수 성공
				unregisterPipeReadEvent(e_udata);
				registerClientSocketWriteEvent(e_udata);
				return;
			}
			else if (pid == 0)
			{
				//종료된 자식프로세스가 없다는 것. 그냥 리턴하고 다음번에 다시 확인하도록 합니다.
				return;
			}
			else 
			{
				//자식 회수 실패
				e_udata->setStatusCode(500);
				unregisterPipeReadEvent(e_udata);
				registerClientSocketWriteEvent(e_udata);
				return;
			}
		}
		else
		{
			EventLoop::getInstance().pipeBuffer[read_len] = '\0';
			static_cast<responseHandler *>(e_udata->getResponseHandler())->setResBody(EventLoop::getInstance().pipeBuffer);
		}
	}
}

void EventLoop::e_fileReadCallback(struct kevent *e, Event *e_udata)
{
	if (e_udata->getServerType() == HTTP_SERVER)
	{
		//read from file
		ssize_t read_len = read(e_udata->file_fd, HttpServer::getInstance().getHttpBuffer(), HTTPBUFFER_SIZE - 1);

		if (read_len == -1)
		{
			e_udata->setStatusCode(500);
			unregisterFileReadEvent(e_udata);
			registerClientSocketWriteEvent(e_udata);
			return;
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
