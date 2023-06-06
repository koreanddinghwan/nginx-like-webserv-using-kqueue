#include "EventLoop.hpp"
#include <sys/errno.h>

void EventLoop::writeCallback(struct kevent *e)
{
	std::cout << "\033[32m"; 
	std::cout<<"write callback"<<std::endl;

	//set event
	Event *e_udata = static_cast<Event *>(e->udata);

	if (e_udata->w_clientEvent)
		e_clientSocketWriteCallback(e, e_udata);
	if (e_udata->w_pipeEvent)
		e_pipeWriteCallback(e, e_udata);
	if (e_udata->w_fileEvent)
		e_fileWriteCallback(e, e_udata);
}

void EventLoop::e_clientSocketWriteCallback(struct kevent *e, Event *e_udata)
{
	std::cout << "\033[33m"; 
	std::cout<<"CLIENT SOCKET WRITE CALLBACK"<<std::endl;
	//we need to verify http
	if (e_udata->getServerType() == HTTP_SERVER)
	{
		/**
		 * size of e->data만큼 작성
		 * */
		int size = static_cast<responseHandler *>(e_udata->getResponseHandler())->getResBuf().length();
		int wroteByte = write(e_udata->getClientFd(), static_cast<responseHandler *>(e_udata->getResponseHandler())->getResBuf().c_str() + e_udata->wrote, size - e_udata->wrote);

		
		/* write(1, static_cast<responseHandler *>(e_udata->getResponseHandler())->getResBuf().c_str() + e_udata->wrote, size - e_udata->wrote); */
		if (wroteByte == -1)
		{
			if (errno == EWOULDBLOCK || errno == EAGAIN)
			{
				std::cout<<"EWOULDBLOCK"<<std::endl;
				return;
			}
			else
			{
				std::cout<<"write error"<<std::endl;
				std::cout<<"errno : "<<errno<<std::endl;
			}
		}

		else
		{
			/**
			 * when this event registered, 
			 * 1. response message created
			 * 2. wrote is 0.
			 * prevent partial write
			 * */
			e_udata->wrote += wroteByte; 
			if (e_udata->wrote == size)
			{
				std::cout<<"wrote all the data"<<std::endl;
				/**
				 * if all the data wrote, unregister write event
				 * */
				unregisterClientSocketWriteEvent(e_udata);
			}
		}
	}
}

void EventLoop::e_pipeWriteCallback(struct kevent *e, Event *e_udata)
{
	std::cout << "\033[35m"; 
	std::cout<<"pipe Write callback"<<std::endl;
	if (e_udata->getServerType() == HTTP_SERVER)
	{
		HttpreqHandler *reqHandler = static_cast<HttpreqHandler *>(e_udata->getRequestHandler());
		if ((reqHandler->_contentLength) == 0 && 
				!reqHandler->getIsPending())
		{
			/**
			 * if there are no data to be read, unregister read event
			 * */
			unregisterPipeWriteEvent(e_udata);
			return;
		}

		/**
		 * todo : file size
		 * */
		int fileSize = static_cast<HttpreqHandler *>(e_udata->getRequestHandler())->_contentLength;

		/**
		 * todo : file buffer
		 * */
		int wroteByte = write(e_udata->PtoCPipe[1], static_cast<HttpreqHandler *>(e_udata->getRequestHandler())->getRequestInfo().body.c_str() + e_udata->fileWroteByte, fileSize - e_udata->fileWroteByte);

		if (wroteByte == -1)
		{
			if (errno == EAGAIN|| errno == EWOULDBLOCK)
			{
				std::cout<<"there are no data to be read"<<std::endl;
				return;
			}
			else
			{
				std::cout<<"UNKNOWN ERROR"<<std::endl;
				std::cout<<"Errno: "<<errno<<std::endl;
				e_udata->setStatusCode(500);
				unregisterPipeWriteEvent(e_udata);
			}
		}
		else if (wroteByte == 0)
		{
			if (reqHandler->getIsPending())
				return ;
			else
				unregisterPipeWriteEvent(e_udata);
		}
		else
		{
			std::cout<<"wrote byte:"<<wroteByte<<std::endl;
			std::cout<<"wroteByte : "<<e_udata->fileWroteByte<<std::endl;
			//update wrote byte
			e_udata->fileWroteByte += wroteByte;

			//if all the data wrote, unregister write event
			// change if (u_udata->getRequestHandler->getRequestInfo.fileEOF &&)
			if (e_udata->fileWroteByte == fileSize)
			{
				//end
				e_udata->setStatusCode(200);
				unregisterPipeWriteEvent(e_udata);
			}
		}
	}
}

void EventLoop::e_fileWriteCallback(struct kevent *e, Event *e_udata)
{
	std::cout << "\033[33m"; 
	std::cout<<"FILE WRITE CALLBACK"<<std::endl;
	if (e_udata->getServerType() == HTTP_SERVER)
	{
		/**
		 * todo : file size
		 * */
		int fileSize = static_cast<HttpreqHandler *>(e_udata->getRequestHandler())->_contentLength;

		std::cout<<"file size : "<<fileSize<<std::endl;
		/**
		 * todo : file buffer
		 * */
		int wroteByte = write(e_udata->file_fd, static_cast<HttpreqHandler *>(e_udata->getRequestHandler())->getRequestInfo().body.c_str() + e_udata->fileWroteByte, fileSize - e_udata->fileWroteByte);

		if (wroteByte == -1)
		{
			if (errno == EAGAIN)
			{
				std::cout<<"there are no data to be read"<<std::endl;
				return;
			}
			else
			{
				std::cout<<"UNKNOWN ERROR"<<std::endl;
				std::cout<<"Errno: "<<errno<<std::endl;
				e_udata->setStatusCode(500);
				unregisterFileWriteEvent(e_udata);
				registerFileWriteEvent(e_udata);
			}
		}
		else
		{
			//update wrote byte
			e_udata->fileWroteByte += wroteByte;

			//if all the data wrote, unregister write event
			// change if (u_udata->getRequestHandler->getRequestInfo.fileEOF &&)
			
			if (e_udata->fileWroteByte == fileSize &&
					!static_cast<HttpreqHandler *>(e_udata->getRequestHandler())->getIsPending())
			{
				e_udata->setStatusCode(201);
				unregisterFileWriteEvent(e_udata);
				registerClientSocketWriteEvent(e_udata);
			}
		}

	}
}
