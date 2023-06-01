#include "../eventLoop/EventLoop.hpp"
#include <cstdint>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include "../http/ws_HttpIndexModule.hpp"
#include "../http/ws_HttpAutoIndexModule.hpp"
#include "../http/ws_HttpUploadModule.hpp"
#include "../http/HttpReqHandler.hpp"
#include "Event.hpp"

/**
 * handle error
 *
 * 1. check host name in server names
 * 	-> and choose server data to use
 * 2. set the location data to use by checking the host's uri in location datas by prefix matching 
 * 3. check the allowed_mtehods in location data
 * 4. make resource route in server with root route
 * 5. if redirecturl exists, redirect to url
 * 		-> set status code and redirection url.
 * */
void EventLoop::setHttpResponse(Event *e)
{
	HttpreqHandler *reqHandler = static_cast<HttpreqHandler *>(e->getRequestHandler());


	/**
	 * 1. check host name in server names
	 * */
	// 효율 안좋다 ..
	std::string host = reqHandler->getRequestInfo().reqHeaderMap.find("Host")->second;
	for (int i = 0; i < e->getServerData()->size(); i++)
	{
		std::vector<std::string>& serverNames = (e->getServerData()->at(i)->getServerNames());
		for (int j = 0; j < serverNames.size(); j++)
		{
			//i번째 serverData에 있는 server names에서 host가 있으면
			if (host == serverNames[j])
			{
				//사용할 serverData를 i번째로 바꿔준다.
				e->setDefaultServerData(e->getServerData()->at(i));
				e->serverName = serverNames[j];
				break ;
			}
		}
	}

	/**
	 * 2. check the host's uri in location datas by prefix matching
	 * */
	std::string requestPath = reqHandler->getRequestInfo().path;
	e->locationData = NULL;
	int matchScore = -1;
	for (int i = 0; i < e->getDefaultServerData()->getLocationDatas().size(); i++)
	{
		std::string &locationUri = e->getDefaultServerData()->getLocationDatas().at(i)->getUri();
		if (matchScore < getLongestPrefixMatchScore(locationUri, requestPath))
		{
			matchScore = getLongestPrefixMatchScore(locationUri, requestPath);
			e->locationData = e->getDefaultServerData()->getLocationDatas().at(i);
		}
	}
	if (!e->locationData)
	{
		std::cout<<"no location data"<<std::endl;
		e->setStatusCode(404);
		throw std::exception();
	}
	std::cout<<"============location setted============="<<std::endl;
	e->locationData->printLocationData();

	/**
	 * 3. check the allowed_methods in location data
	 * */
	int methodIndex = MethodFactory::getInstance().getMethodIndex(reqHandler->getRequestInfo().method);
	if (!(e->locationData->getLimitedMethods().methods[methodIndex]))
	{
		/**
		 * 405: "method not allowed"
		 **/
		e->setStatusCode(405);
		/**
		 * throw exception.
		 * if event loop catch this exception, then the httpexception event will be registered.
		 * */
		throw std::exception();
	}

	/**
	 * check clientMaxBodySize
	 * */
	if (e->locationData->getClientMaxBodySize() < reqHandler->getRequestInfo().body.length())
	{
		e->setStatusCode(413);
		throw std::exception();
	}

	/**
	 * 4. make resource route in server with root route
	 * */

	int pos;
	std::string tmp;

	if (e->locationData->getUri() == "/")
	{
		e->setResource(requestPath);
		e->setRoute(e->locationData->getRoot() + e->getResource());
	}

	// 1. check if the requested resource is directory
	else if (requestPath.back() == '/')
	{
		if (requestPath.length() == 1)
		{
			e->setResource("/");
			e->setRoute(e->locationData->getRoot() + e->getResource());
		}
		else
		{
			if (e->locationData->getUri() == requestPath)
			{
				/**
				 * path : /test/
				 * loc  : /test/
				 * resource : /
				 * route : root + /
				 * */
				e->setResource("/");
				e->setRoute(e->locationData->getRoot() + e->getResource());
			}
			else if (e->locationData->getUri().back() == '/')
			{
				/**
				 * request path is directory and location uri is directory
				 * */
				/**
				 * path : /test/
				 * loc  : /
				 * resource : test/
				 * route : root + / + test/
				 * */
				/**
				 * path : /test/abc/
				 * loc  : /test/
				 * resource : abc/
				 * route : root + / + abc/
				 * */
				tmp = requestPath.substr(1);
				pos = tmp.find("/");
				e->setResource(requestPath.substr(pos + 1));
				e->setRoute(e->locationData->getRoot() + "/" + e->getResource());
			}

			else {
				/**
				 * request path is directory but location uri is not directory
				 * */
				/**
				 * path : /test/
				 * loc  : /t
				 * resource : /
				 * route : root + /
				 * */
				/**
				 * path : /test/
				 * loc  : /test
				 * resource : /
				 * route : root + /
				 * */
				/**
				 * path : /test/abc/
				 * loc  : /testasdfasdfa
				 * resource : /abc/
				 * route : root + /abc//
				 * */
				tmp = requestPath.substr(1);
				std::cout<<tmp<<std::endl;
				pos = tmp.find("/");
				e->setResource(requestPath.substr(pos));
				e->setRoute(e->locationData->getRoot() + e->getResource());
			}
		}
	}
	// 2. check if the requested resource is file
	else
	{
		/**
		 * request path is file and location uri is not directory
		 * */
		if (e->locationData->getUri().back() != '/')
		{
			if (e->locationData->getUri() == requestPath)
			{
				/**
				 * request path and location uri is equal
				 * */
				/**
				 * path : /test
				 * loc  : /test
				 * resource : ""
				 * route : root + ""
				 * */
				e->setResource("");
				e->setRoute(e->locationData->getRoot() + e->getResource());
			}
			else if (
					requestPath.substr(1)\
					.find("/") == std::string::npos)
			{
				/**
				 * resource identifier not exists
				 * */
				/**
				 * path : /test
				 * loc  : /t
				 * resource : ""
				 * route : root + ""
				 * */
				e->setResource("");
				e->setRoute(e->locationData->getRoot() + e->getResource());
			}
			else {
				/**
				 * resource identifier exists
				 * */
				/**
				 * path : /test/abc
				 * loc  : /tes
				 * resource : abc
				 * route : root + / + abc
				 * */
				/**
				 * path : /test/abc
				 * loc  : /test
				 * resource : abc
				 * route : root + / + abc
				 * */
				tmp = requestPath.substr(1);
				pos = tmp.find("/");
				e->setResource(tmp.substr(pos + 1));
				e->setRoute(e->locationData->getRoot() + "/" + e->getResource());
			}
		}
		else 
		{
			/**
			 * request path is file but location uri is directory
			 * */
			/**
			 * path : /test/abc
			 * loc  : /test/
			 * resource : abc
			 * route : root + "/" + abc
			 * */
			/**
			 * path : /test/abc
			 * loc  : /
			 * resource : test/abc
			 * route : root + test/abc
			 * */
			tmp = requestPath.substr(1);
			pos = tmp.find("/");
			e->setResource(tmp.substr(pos + 1));
			e->setRoute(e->locationData->getRoot() + "/" + e->getResource());
		}
	}
	std::cout<<"=======!!!!!!!!!!!!!!!!!!!!!!!!!11!!!!=====resource setted============="<<std::endl;
	std::cout<<"route: "<<e->getRoute()<<std::endl;
	std::cout<<"resource: "<<e->getResource()<<std::endl;
	std::cout<<"=======!!!!!!!!!!!!!!!!!!!!!!!!!!!1================================"<<std::endl;

	/**
	 * 5. if redirecturl exists, redirect to url
	 * */
	if (!e->locationData->getRedirectUrl().empty())
	{
		e->setStatusCode(e->locationData->getReturnStatus());
		throw std::exception();
	}

	/**
	 * 6. if need cgi process
	 * */
	if (!e->locationData->getCgiPass().empty())
	{
		/**
		 * process cgi
		 * */
		//process cgi
		if (!processCgi(e))
			throw std::exception();
		return ;
	}

	/**
	 * 7. process methods
	 * */

	/**
	 * if method == GET
	 * */
	if (methodIndex == GET || methodIndex == HEAD)
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
		else
		{
		/**
		 * 2. resource is file
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
	}
	
	if (methodIndex == POST)
	{
		std::cout<<"method is POST"<<std::endl;
		if (!e->locationData->getUploadStore().empty())
		{
			if (ws_HttpUploadModule::processEvent(e) == false)
				throw std::exception();
			unregisterClientSocketReadEvent(e);
			registerFileWriteEvent(e);
			return ;
		}
		/**
		 * upload pass not setted in location block
		 * */
		else
		{
			e->setStatusCode(200);
			unregisterClientSocketReadEvent(e);
			registerClientSocketWriteEvent(e);
			return ;
		}
	}

	if (methodIndex == DELETE)
	{
		std::cout<<"method is DELETE"<<std::endl;
		/**
		 * check if the resource exists
		 * */
	}
	e->setStatusCode(404);
}

int EventLoop::getLongestPrefixMatchScore(const std::string& location, const std::string& requestPath) {
	if (location.back() == '/')
	{
		if (location == requestPath)
			return INT32_MAX;
	}
    size_t length = std::min(location.length(), requestPath.length());

	/**
	 * @TODO 
	 * if location uri ends with /
	 * */
    // Find the index where the location and requestPath differ
    size_t index = 0;
    while (index < length && location[index] == requestPath[index]) {
        index++;
    }
    // Return the matched prefix length
    return location.substr(0, index).length();
}

void setEnv(Event *e)
{
	HttpreqHandler *reqHandler = static_cast<HttpreqHandler *>(e->getRequestHandler());
	e->getCgiEnv()[0]->append("AUTH_TYPE=Basic");
	e->getCgiEnv()[1]->append("CONTENT_LENGTH=");
	e->getCgiEnv()[1]->append(reqHandler->getRequestInfo().reqHeaderMap.find("Content-Length")->second);
	e->getCgiEnv()[2]->append("CONTENT_TYPE=").append(reqHandler->getRequestInfo().reqHeaderMap.find("Content-Type")->second);
	e->getCgiEnv()[3]->append("GATEWAY_INTERFACE=CGI/1.1");
	e->getCgiEnv()[4]->append("PATH_INFO=").append(e->getResource());
	e->getCgiEnv()[5]->append("PATH_TRANSLATED=").append(e->getRoute());
	e->getCgiEnv()[6]->append("QUERY_STRING=");
	e->getCgiEnv()[7]->append("REMOTE_ADDR=").append(inet_ntoa(e->getSocketInfo().socket_addr.sin_addr));
	e->getCgiEnv()[8]->append("REMOTE_HOST=");
	e->getCgiEnv()[9]->append("REMOTE_IDENT=");
	e->getCgiEnv()[10]->append("REMOTE_USER=");
	e->getCgiEnv()[11]->append("REQUEST_METHOD=").append(reqHandler->getRequestInfo().method);
	e->getCgiEnv()[12]->append("SCRIPT_NAME=").append(e->getDir().append(e->locationData->getCgiPass()));
	e->getCgiEnv()[13]->append("SERVER_NAME=").append(e->serverName);
	char pt[10];
    sprintf(pt, "%d", e->getDefaultServerData()->getListen());
	e->getCgiEnv()[14]->append("SERVER_PORT=").append(pt);
	e->getCgiEnv()[15]->append("SERVER_PROTOCOL=").append(reqHandler->getRequestInfo().httpVersion);
	e->getCgiEnv()[16]->append("SERVER_SOFTWARE=webserv/1.0");
	e->getCgiEnv()[17]->append("scheme=http");
	e->getCgiEnv()[18]->append("protocol-var-name=HTTP/1.1");
	e->getCgiEnv()[19]->append("extension-var-name=");
}

bool EventLoop::processCgi(Event *e)
{
	/**
	 * first, check if the cgi file exists
	 * */
	if (stat(e->getRoute().c_str(), &e->statBuf) != 0)
	{
		std::cout << "stat error" << std::endl;
		e->setStatusCode(404);
		return false;
	}


	HttpreqHandler *reqHandler = static_cast<HttpreqHandler *>(e->getRequestHandler());
	setEnv(e);

	/**
	 * 1. create pipe
	 * */
	int *pipefd = e->getPipeFd();
	if (pipe(pipefd) == -1)
	{
		close(pipefd[0]);
		close(pipefd[1]);
		e->setStatusCode(500);
		std::cout << "pipe error" << std::endl;
		return false;
	}

	/**
	 * 2. set non blocking to pipe
	 * */
	if (fcntl(pipefd[0], F_SETFL, O_NONBLOCK) == -1)
	{
		close(pipefd[0]);
		close(pipefd[1]);
		e->setStatusCode(500);
		std::cout << "fcntl error" << std::endl;
		return false;
	}

	if (fcntl(pipefd[1], F_SETFL, O_NONBLOCK) == -1)
	{
		close(pipefd[0]);
		close(pipefd[1]);
		e->setStatusCode(500);
		std::cout << "fcntl error" << std::endl;
		return false;
	}

	/**
	 * 3. fork
	 * */
	pid_t pid = fork();
	if (pid == -1)
	{
		close(pipefd[0]);
		close(pipefd[1]);
		e->setStatusCode(500);
		std::cout << "fork error" << std::endl;
		return false;
	}


	/**
	 * 4. parent process
	 * */
	if (pid)
	{
		//close write end
		close(pipefd[1]);
		pipefd[1] = -1;

		e->setEventType(E_PIPE);
		//unregisterClientSocketReadEvent(e);
		unregisterClientSocketReadEvent(e);
		//set pipe read event
		registerPipeReadEvent(e);
		return true;
	}
	/**
	 * 5. child process
	 * */
	else 
	{
		//close read end
		close(pipefd[0]);
		// 표준 출력을 파이프의 쓰기용 파일 디스크립터로 리디렉션
        dup2(pipefd[1], STDOUT_FILENO);

		//실행
		char **env = new char*[e->getCgiEnv().size() + 1];
		for (size_t i = 0; i < e->getCgiEnv().size(); i++)
			env[i] = const_cast<char *>(e->getCgiEnv()[i]->c_str());
		env[e->getCgiEnv().size()] = NULL;
		if (execve(e->getRoute().c_str(), NULL, env) == -1)
		{
			close(pipefd[1]);
			e->setStatusCode(404);
			exit(1);
		}
	}
}
