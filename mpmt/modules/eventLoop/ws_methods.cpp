#include "EventLoop.hpp"

void EventLoop::ws_method_GET(Event *e)
{
	std::cout<<"=============================="<<std::endl;
	std::cout<<"method is GET or HEAD"<<std::endl;
	std::cout<<"root + internal uri: "<<std::endl;
	std::cout<<e->locationData->getRoot() + e->internal_uri<<std::endl;
	if ((stat((e->locationData->getRoot() + e->internal_uri).c_str(), &e->statBuf) == 0) &&
			(e->statBuf.st_mode & S_IFREG) &&
			(e->file_fd = open((e->locationData->getRoot() + e->internal_uri).c_str(), O_RDONLY)) != -1)
	{
		if (fcntl(e->file_fd, F_SETFL, O_NONBLOCK) == -1)
		{
			std::cout<<"fcntl error"<<std::endl;
			e->setStatusCode(500);
			errorCallback(e);
			return ;
		}
		
		if (e->statBuf.st_size == 0)
		{
			std::cout<<"file size is 0"<<std::endl;
			e->setStatusCode(204);
			unregisterClientSocketReadEvent(e);
			registerClientSocketWriteEvent(e);
			return ;
		}
		e->setStatusCode(200);
		unregisterClientSocketReadEvent(e);
		registerFileReadEvent(e);
		return ;
	}
	else
	{
		if (!(e->locationData->getIndex().empty()))
		{
			std::cout<<"check index setted"<<std::endl;
			std::cout<<"|||||||||"<<"index exists"<<"|||||||||"<<std::endl;
			if (ws_HttpIndexModule::processEvent(e))
			{
				/**
				 * index module에서 처리 완료하면, 다시 internal 리디렉션해야함.
				 * file open처리는 다시 internal 리디렉션된 이벤트에서 처리
				 * */
				std::cout<<"internal redirection"<<std::endl;
				setHttpResponse(e);
				return ;
			}
		}
		/**
		 * index module에서 처리 못하면 autoindex에서 처리
		 * */
		if (e->internal_uri.back() == '/' && e->locationData->getAutoIndex())
		{
			std::cout<<"check autoindex setted"<<std::endl;
			if (ws_HttpAutoIndexModule::processEvent(e))
			{
				/**
				 * resBody에 autoindex내용 들어가있음.
				 * */
				unregisterClientSocketReadEvent(e);
				registerClientSocketWriteEvent(e);
				return ;
			}
		}
		std::cout<<"file open error"<<std::endl;
		e->setStatusCode(404);
		errorCallback(e);
		return ;
	}
}

void EventLoop::ws_method_POST(Event *e)
{
	std::cout<<"method is POST"<<std::endl;
	if (!e->locationData->getUploadStore().empty())
	{
		/**
		 * upload store is setted
		 * */
		if (ws_HttpUploadModule::processEvent(e))
		{
			unregisterClientSocketReadEvent(e);
			registerFileWriteEvent(e);
			return ;
		}
		else
		{
			unregisterClientSocketReadEvent(e);
			registerClientSocketWriteEvent(e);
		}
	}
	else
	{
		/**
		 * Upload store is not setted
		 * std::cout<<"upload store is not setted"<<std::endl;
		 * */
		e->setStatusCode(404);
		errorCallback(e);
	}
}

void EventLoop::ws_method_DELETE(Event *e)
{
	std::cout<<"method is DELETE"<<std::endl;
	/**
	 * check if the resource exists
	 * */
	std::string filePath;
	filePath = e->locationData->getRoot() + e->internal_uri;
}
