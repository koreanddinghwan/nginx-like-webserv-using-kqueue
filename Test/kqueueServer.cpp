#include <sys/_types/_iovec_t.h>
#include <sys/_types/_size_t.h>
#include <sys/errno.h>
#include <sys/signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <sys/event.h>
#include <sys/fcntl.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <signal.h>

# define WORKER_CONNECTIONS 1024
# define MAX_EVENT 256

int asdfasd = 0;

int main()
{
    int kq_fd = kqueue();
    if (kq_fd == -1) {
        std::cerr << "Failed to create kqueue\n";
        return 1;
    }

//////////////////////////////////////server block
///server module에서 처리
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        std::cerr << "Failed to create socket\n";
        return 1;
    }


	//소켓 종료이후에도 동일포트, 주소 사용하게설정
	int opt = 1;
	setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT | SO_REUSEADDR, &opt, sizeof(opt));

	const char *addr = "127.0.0.1";

    struct sockaddr_in address;
    address.sin_family = AF_INET;
	if (!inet_aton(addr, &address.sin_addr))
	{	
        std::cerr << "Failed to set address"<<addr<<std::endl;
		return 1;
	}
    address.sin_port = htons(8000);


    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == -1) {
        std::cerr << "Failed to bind to port 8080\n";
        return 1;
    }

    if (listen(server_fd, WORKER_CONNECTIONS) == -1) {
        std::cerr << "Failed to listen on socket\n";
        return 1;
    }


	fcntl(server_fd, F_SETFL, O_NONBLOCK);

	struct kevent kev;
	/*
	 * EVFILT_READ: 파일디스크립터를 event identifier로 정한다.
	 * socket에 설정하면 pending 상태에 있는 connection이 server_fd에 있을때 트리거된다.
	 * EV_ADD는 kev에 이 이벤트 트리거를 추가한다는 의미.
	 * */
    EV_SET(&kev, server_fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
    if (kevent(kq_fd, &kev, 1, NULL, 0, NULL) == -1) {
        std::cerr << "Failed to register server socket with kqueue\n";
        return 1;
    }

    std::cout << "Server started on port 8000\n";
//////////////////////////////////serverblock end



    while (true) {
        struct kevent events[MAX_EVENT];
		//pending 상태의 kqueue의 이벤트를 kevent로 가져온다.
        int nevents = kevent(kq_fd, NULL, 0, events, MAX_EVENT, NULL);
        if (nevents == -1) {
            std::cerr << "Failed to wait for events\n";
            return 1;
        }

        for (int i = 0; i < nevents; ++i) 
		{
			//server_fd에서 발생한 이벤트, 즉 새로운 client connection을 감지.
			//client fd를 read event를 등록한다.
			//client_fd가 read할 준비가 되면 event에 추가될것.
            if (events[i].ident == server_fd) 
			{
                struct sockaddr_in client_address;
                socklen_t client_len = sizeof(client_address);
                int client_fd = accept(server_fd, (struct sockaddr *)&client_address, &client_len);
				std::cout<<client_fd<<std::endl;
                if (client_fd == -1) {
                    std::cerr << "Failed to accept client connection\n";
                    continue;
                }

				//client socket에 nonblock mode명시
                if (fcntl(client_fd, F_SETFL, O_NONBLOCK) == -1)
				{
					std::cerr<<"Non blocking err"<<std::endl;
					close(client_fd);
					continue;
				}

				//tcp keepalive
				//소켓 종료이후에도 동일포트, 주소 사용하게설정
				int opt = 1;
				setsockopt(client_fd, SOL_SOCKET, SO_REUSEPORT | SO_REUSEADDR, &opt, sizeof(opt));

				// Enable TCP keepalive
				int enable_keepalive = 1;
				setsockopt(client_fd, SOL_SOCKET, SO_KEEPALIVE, &enable_keepalive, sizeof(enable_keepalive));

				// Set the idle time before sending keepalive packets to 5 seconds
				int idle_time = 5;
				setsockopt(client_fd, IPPROTO_TCP, TCP_KEEPALIVE, &idle_time, sizeof(idle_time));

				// Set the interval between keepalive packets to 10 seconds
				int interval = 2;
				setsockopt(client_fd, IPPROTO_TCP, TCP_KEEPINTVL, &interval, sizeof(interval));

				// Set the number of keepalive packets to send before considering the connection dead to 5
				int max_attempts = 1;
				setsockopt(client_fd, IPPROTO_TCP, TCP_KEEPCNT, &max_attempts, sizeof(max_attempts));


				//client socket을 kqueue에 등록
                EV_SET(&kev, client_fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
                if (kevent(kq_fd, &kev, 1, NULL, 0, NULL) == -1) {
                    std::cerr << "Failed to register client socket with kqueue\n";
					continue;
                }

                std::cout << "Client connected\n";
				std::cout<< client_address.sin_family<<std::endl;
				std::cout << (client_address.sin_port)<<std::endl;
				std::cout << inet_ntoa(client_address.sin_addr) <<std::endl;
            }
            else 
			{
				//client로부터 들어온 데이터 처리

				int client_fd = events[i].ident;
				struct sockaddr_in client_address;
				socklen_t client_len = sizeof(client_address);
				char buffer[1024];
				ssize_t n = read(client_fd, buffer, sizeof(buffer));
				std::cout<<client_fd<<std::endl;

				if (n == -1) 
				{
					if (errno == EAGAIN || errno == EWOULDBLOCK)
						continue;
					else
					{
						std::cerr << "Failed to read from client socket\n";
						continue;
					}
				}
				else if (n == 0) 
				{
					std::cout << "Client disconnected\n";
					//client 소켓 정리
					asdfasd++;
					EV_SET(&kev, client_fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
					close(client_fd);
					std::cout<<asdfasd<<std::endl;
				}
				else 
				{
					std::cout << "Received " << n << " bytes from client\n";
					std::cout<<buffer<<std::endl;

				/*
				 * parsing http protocol.
				 * 1. 정적파일 제공
				 * 2. cgi 통해 동적파일 제공
				 * */

				//tcp keepalive
				//소켓 종료이후에도 동일포트, 주소 사용하게설정
				int opt = 1;
				// Enable TCP keepalive
				int enable_keepalive = 1;
				// Set the idle time before sending keepalive packets to 30 seconds
				int idle_time = 5;
				// Set the interval between keepalive packets to 10 seconds
				int interval = 1;
				// Set the number of keepalive packets to send before considering the connection dead to 5
				int max_attempts = 2;
				if (setsockopt(client_fd, SOL_SOCKET, SO_REUSEPORT | SO_REUSEADDR, &opt, sizeof(opt)) == -1 ||
				setsockopt(client_fd, SOL_SOCKET, SO_KEEPALIVE, &enable_keepalive, sizeof(enable_keepalive)) == -1 ||
				setsockopt(client_fd, IPPROTO_TCP, TCP_KEEPALIVE, &idle_time, sizeof(idle_time)) == 1 ||
				setsockopt(client_fd, IPPROTO_TCP, TCP_KEEPINTVL, &interval, sizeof(interval)) == -1 ||
				setsockopt(client_fd, IPPROTO_TCP, TCP_KEEPCNT, &max_attempts, sizeof(max_attempts)) == -1)
				{
					std::cout<<"errno:" <<std::endl;
				}



					// Process incoming data according to the HTTP protocol
					// Generate a response based on the incoming request
					// Write the response to the client socket

					// Example response
					// content length안지키면 client측에서 block되는 문제있음. 주의
					std::string response = "HTTP/1.1 200 OK\r\nContent-Length: 5100\r\n\r\naaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaa";
					
					size_t byte_sent = 0;
					size_t byte_left = response.length();
					while (byte_left > 0)
					{
						int n = write(client_fd, response.c_str(), response.length());
						if (n == -1)
						{
							if (errno == EWOULDBLOCK || errno == EAGAIN)
							{
								struct kevent event[1];
								EV_SET(&event[0], client_fd, EVFILT_WRITE, EV_ADD, 0, 0, NULL);
								if (kevent(kq_fd, events, 1, NULL, 0, NULL) == -1) {
								std::cerr << "Failed to register client socket with kqueue\n";
								return 1;
								}
								// Wait for the socket to become writable
								nevents = kevent(kq_fd, NULL, 0, events, 1, NULL);
								if (nevents == -1) {
									std::cerr << "Failed to wait for events\n";
									return 1;
								}
							} else {
								break;
							}
						}
						else 
						{
							byte_sent += n;
							byte_left -= n;
						}
					}
				}
			}
		}
	}
	return 0;
}
