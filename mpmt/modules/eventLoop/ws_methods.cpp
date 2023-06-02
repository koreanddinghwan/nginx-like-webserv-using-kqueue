#include "EventLoop.hpp"
#include <exception>

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
			if (ws_HttpIndexModule::processEvent(e) == false)
				throw std::exception();
			else
			{
				//pipelining do not used in these days
				//https://en.wikipedia.org/wiki/HTTP_pipelining
				unregisterClientSocketReadEvent(e);
				registerFileReadEvent(e);
				return ;
			}
		}
		else if (e->locationData->getAutoIndex())
		{
			std::cout<<"check autoindex setted"<<std::endl;
			if (ws_HttpAutoIndexModule::processEvent(e) == false)
				throw std::exception();
			else
			{
				unregisterClientSocketReadEvent(e);
				registerClientSocketWriteEvent(e);
				return ;
			}
		}
		else {
			std::cout<<"route endwith / but index not setted"<<std::endl;
			e->setStatusCode(404);
			throw std::exception();
		}
	}
	/**
	 * resource is file
	 * */
	std::cout<<"resource is file"<<std::endl;
	
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
}
