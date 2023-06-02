#include "../eventLoop/EventLoop.hpp"
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

	if (requestPath.back() == '/' && \
			requestPath.length() == 1)
	{
		std::cout<<"|||||request path is /"<<std::endl;
		/**
		 * ex) / ->  root + /
		 * */
		e->setRoute(e->locationData->getRoot() + "/");
		e->setDir(e->locationData->getRoot());
		e->setResource("/");
	}
	else
	{
		/**
		 *ex) req    : /test/aser/, 
		 *	  locUri : /t
		 *	  root : /var/www
		 *
		 *	  == /var/www + /aser/
		 * */
		//parse string
		
		std::string tmp;

		tmp = reqHandler->getRequestInfo().path.substr(
				e->locationData->getUri().length()
				);
		std::cout<<"tmp: "<<tmp<<std::endl;
		int pos = tmp.find('/');

		if (pos == std::string::npos)
		{
			e->setRoute(
					e->locationData->getRoot() + \
					+ "/" + tmp
					);
		}
		else
		{	e->setRoute(
					e->locationData->getRoot() + \
					tmp.substr(pos) \
					);
		}
	}
	std::cout<<"=======!!!!!!!!!!!!!!!!!!!!!!!!!11!!!!=====resource setted============="<<std::endl;
	std::cout<<"route: "<<e->getRoute()<<std::endl;
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
		{
			e->setStatusCode(500);
			throw std::exception();
		}
		return ;
	}

	/**
	 * 7. process methods
	 * */

	/**
	 * if method == GET
	 * */
	if (methodIndex == GET)
	{
		std::cout<<"method is GET"<<std::endl;
		/**
		 * 1. check if the requested resource is directory
		 * */
		if (e->getRoute().back() == '/')
		{
			// process directory
			// 1. first, check index
			if (!(e->locationData->getIndex().empty()))
			{
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
			// 2. second, if index not work check autoindex
			else if (e->locationData->getAutoIndex())
			{
				if (ws_HttpAutoIndexModule::processEvent(e) == false)
					throw std::exception();
				else
				{
					unregisterClientSocketReadEvent(e);
					registerFileReadEvent(e);
					return ;
				}
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
			/**
			 * upload store is setted
			 * */
			if (ws_HttpUploadModule::processEvent(e) == false)
			{
				e->setStatusCode(500);
				throw std::exception();
			}
			e->setStatusCode(201);
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

	if (methodIndex == DELETE)
	{
		std::cout<<"method is DELETE"<<std::endl;
		/**
		 * check if the resource exists
		 * */
	}
	e->setStatusCode(404);
	throw std::exception();
}

int EventLoop::getLongestPrefixMatchScore(const std::string& location, const std::string& requestPath) {
    // Find the length of the shortest string
    size_t length = std::min(location.length(), requestPath.length());

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
		std::cout << "pipe error" << std::endl;
		return false;
	}

	/**
	 * 2. set non blocking to pipe
	 * */
	if (fcntl(pipefd[0], F_SETFL, O_NONBLOCK) == -1)
	{
		std::cout << "fcntl error" << std::endl;
		return false;
	}

	if (fcntl(pipefd[1], F_SETFL, O_NONBLOCK) == -1)
	{
		std::cout << "fcntl error" << std::endl;
		return false;
	}

	/**
	 * 3. fork
	 * */
	pid_t pid = fork();
	if (pid == -1)
	{
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
