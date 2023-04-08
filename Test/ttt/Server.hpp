#ifndef SERVER_HPP
# define SERVER_HPP

#include <exception>
#include <iostream>
#include <cstring>
#include <stdexcept>
#include <vector>
#include <unistd.h>
#include "../../interface/IServer.hpp"
#include "../../interface//IModule.hpp"
#include "../config/Config.hpp"

#ifdef MULTITHREADING
#include "./threadpool.hpp"
#endif

const int MAX_EVENTS = 32;
const int BUFFER_SIZE = 4096;
const int WORKER_PROCESSES = 4;

class Server : public IServer
{
private:
	/*
	 * config module: configuration 파일 파싱, 정보 저장
	 * socket modules: configuration 파일로 설정된 정보로 socket 처리
	 * ipc module: master<->slave process간 통신을 위한 ipc소켓처리
	 * http module:
	 * http request: http메세지  파싱 모듈 bonus:cookie확인
	 * http response: http메세지 작성모듈, bonus:cookie확인
	 * */
	IModule	*HttpModule;

	#ifdef MULTIPROCESS
	IModule	*IpcSocketModule;
	#endif


public:
	Server(
			IModule *httpModule
	#ifdef MULTIPROCESS
			,IModule *ipcSocketModule;
	#endif
			):  
		 HttpModule(httpModule)
	#ifdef MULTIPROCESS
		IpcSocket(ipcSocketModule);
	#endif
	{}

	void runServer() throw(std::runtime_error) {



#ifdef MULTIPROCESS
		for (int i = 0; i < WORKER_PROCESSES; i++)
		{
			int pid = fork();
			if (pid < 0)
				throw (std::runtime_error("fork err"));
			if (pid != 0) //master process
			{

			}
			else//worker process
			{
				#ifdef MULTITHREADING
				//set worker thread to processing parrarel I/O
				#endif
				//event loop module(kqueue) needed 
			}
		}
#endif
	}

#ifdef MULTITHREADING
	void setThreadPool() {

	}
#endif

	~Server() {}
};


#endif
