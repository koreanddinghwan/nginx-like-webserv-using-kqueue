#include "EventLoop.hpp"

void EventLoop::ws_method_GET(Event *e) throw (std::exception)
{
	std::cout<<"method is GET or HEAD"<<std::endl;
	/**
	 * 1. check if the requested resource is directory
	 * */
	if (e->getRoute().back() == '/')
	{
		// process directory
		// 1. first, check index
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
				e->getResource().clear();
				e->getRoute().clear();
				setHttpResponse(e);
				return;
			}
		}
		/**
		 * index module에서 처리 못하면 autoindex에서 처리
		 * */
		if (e->locationData->getAutoIndex())
		{
			std::cout<<"check autoindex setted"<<std::endl;
			if (ws_HttpAutoIndexModule::processEvent(e) == false)
				/**
				 * auto index에서 발생한 오류
				 * autoindex module에서 처리 못하면 404
				 * */
				throw std::exception();
			else
			{
				/**
				 * resBody에 autoindex내용 들어가있음.
				 * */
				unregisterClientSocketReadEvent(e);
				registerClientSocketWriteEvent(e);
				return ;
			}
		}
		/**
		 * /로 끝나지만, index도 없고 autoindex도 없으면 404
		 * */
		std::cout<<"route endwith / but index not setted"<<std::endl;
		e->setStatusCode(404);
		throw std::exception();
	}
	/**
	 * resource is file
	 * */
	else 
	{
		std::cout<<"resource is file"<<std::endl;
		
		if (e->getResource() == "")
		{
			std::cout<<"resource is empty"<<std::endl;
			e->setStatusCode(404);
			throw std::exception();
		}

		if ((stat(e->getRoute().c_str(), &e->statBuf) == 0) &&
				(e->file_fd = open(e->getRoute().c_str(), O_RDONLY)) != -1)
		{
			if (fcntl(e->file_fd, F_SETFL, O_NONBLOCK) == -1)
			{
				std::cout<<"fcntl error"<<std::endl;
				e->setStatusCode(500);
				throw std::exception();
			}
			
			if (e->statBuf.st_size == 0)
			{
				std::cout<<"file size is 0"<<std::endl;
				e->setStatusCode(204);
				throw std::exception();
			}
			e->setStatusCode(200);
			if (e->internal_status == -1)
				unregisterClientSocketReadEvent(e);
			registerFileReadEvent(e);
			return ;
		}
		else
		{
			std::cout<<"file open error"<<std::endl;
			e->setStatusCode(404);
			throw std::exception();
		}
	}
}

void EventLoop::ws_method_POST(Event *e) throw(std::exception)
{
	std::cout<<"method is POST"<<std::endl;
	if (!e->locationData->getUploadStore().empty())
	{
		/**
		 * upload store is setted
		 * */
		if (ws_HttpUploadModule::processEvent(e) == false)
			throw std::exception();
		unregisterClientSocketReadEvent(e);
		registerFileWriteEvent(e);
		return ;
	}
	else
	{
		/**
		 * Upload store is not setted
		 * */
		e->setStatusCode(404);
		throw std::exception();
	}
}

void EventLoop::ws_method_DELETE(Event *e) throw (std::exception)
{
	std::cout<<"method is DELETE"<<std::endl;
	/**
	 * check if the resource exists
	 * */
	std::string filePath;
	if (e->getResource().front() == '/')
		filePath = e->locationData->getRoot() + e->getResource();



}
