#ifndef SERVER_HPP
# define SERVER_HPP

#include <exception>
#include <iostream>
#include <cstring>
#include <vector>
#include "./IServer.hpp"
#include "ISocket.hpp"
#include "Socket.hpp"

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


	/**
	 * @brief configuration file로 설정합니다.
	 */
	void setServerConf() {}

	/**
	 * @brief conf파일, define에 맞춰 소켓을 세팅합니다.
	 */
	void setServerSockets() {}


	void makeWorkerProcess() {

	}

#ifdef MULTITHREADING
	void setThreadPool() {

	}
#endif

	~Server() {}
};


#endif
