#include "EventLoop.hpp"

void EventLoop::writeCallback(struct kevent *e)
{
	//set event
	Event *e_udata = static_cast<Event *>(e->udata);

	switch (e_udata->getEventType()){
		case E_CLIENT_SOCKET:
			e_clientSocketWriteCallback(e, e_udata);
			break;
		//check is pipe read
		case E_PIPE:
			e_pipeWriteCallback(e, e_udata);
			break;
		//check is file read
		case E_FILE:
			e_fileWriteCallback(e, e_udata);
			break;
		default:
			break;
	}
}

void EventLoop::e_clientSocketWriteCallback(struct kevent *e, Event *e_udata)
{
	//we need to verify http
	if (e_udata->getServerType() == HTTP_SERVER)
	{
		/**
		 * size of e->data만큼 작성
		 * */
		int size = static_cast<responseHandler *>(e_udata->getResponseHandler())->getResBuf().length();
		int wroteByte = write(e_udata->getClientFd(), static_cast<responseHandler *>(e_udata->getResponseHandler())->getResBuf().c_str() + e_udata->wrote, size - e_udata->wrote);
		if (wroteByte == -1)
		{
			return;
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

	if (e_udata->getServerType() == HTTP_SERVER)
	{
		if (static_cast<HttpreqHandler *>(e_udata->getRequestHandler())->getRequestInfo().body.length() == 0)
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
		int fileSize = static_cast<HttpreqHandler *>(e_udata->getRequestHandler())->getRequestInfo().body.length();

		/**
		 * todo : file buffer
		 * */
		int wroteByte = write(e_udata->PtoCPipe[1], static_cast<HttpreqHandler *>(e_udata->getRequestHandler())->getRequestInfo().body.c_str() + e_udata->fileWroteByte, fileSize - e_udata->fileWroteByte);

		if (wroteByte == -1)
		{
			e_udata->setStatusCode(500);
			unregisterPipeWriteEvent(e_udata);
		}
		else if (wroteByte == 0)
			return ;
		else
		{
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
	if (e_udata->getServerType() == HTTP_SERVER)
	{
		/**
		 * todo : file size
		 * */
		int fileSize = static_cast<HttpreqHandler *>(e_udata->getRequestHandler())->getRequestInfo().body.length();

		/**
		 * todo : file buffer
		 * */
		int wroteByte = write(e_udata->file_fd, static_cast<HttpreqHandler *>(e_udata->getRequestHandler())->getRequestInfo().body.c_str() + e_udata->fileWroteByte, fileSize - e_udata->fileWroteByte);

		if (wroteByte == -1)
		{
			e_udata->setStatusCode(500);
			unregisterFileWriteEvent(e_udata);
			registerFileWriteEvent(e_udata);
		}
		else
		{
			//update wrote byte
			e_udata->fileWroteByte += wroteByte;

			//if all the data wrote, unregister write event
			// change if (u_udata->getRequestHandler->getRequestInfo.fileEOF &&)
			if (e_udata->fileWroteByte == fileSize)
			{
				e_udata->setStatusCode(201);
				unregisterFileWriteEvent(e_udata);
				registerClientSocketWriteEvent(e_udata);
			}
		}

	}
}
