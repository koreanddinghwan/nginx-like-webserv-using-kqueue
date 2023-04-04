#ifndef SERVER_HPP
# define SERVER_HPP

#include <exception>
#include <iostream>
#include <cstring>
#include <vector>
#include <unistd.h>
#include "./IServer.hpp"
#include "ISocket.hpp"


#ifdef MULTITHREADING
#include "./threadpool.hpp"
#endif

const int MAX_EVENTS = 32;
const int BUFFER_SIZE = 4096;
const int WORKER_PROCESSES = 4;

class Server : public IServer
{
private:
	std::vector<ISocket *> sockets;


public:
	Server() {}

	void run() {
		for (int i = 0; i < WORKER_PROCESSES; i++)
		{
			int pid = fork();
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
	}

#ifdef MULTITHREADING
	void setThreadPool() {

	}
#endif

	~Server() {}
};


#endif
