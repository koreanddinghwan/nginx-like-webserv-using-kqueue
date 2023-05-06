#include <cstring>
#include <sys/_types/_iovec_t.h>
#include <sys/_types/_size_t.h>
#include <sys/_types/_uintptr_t.h>
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
# define MAX_EVENT 12

/**
 * @brief kevent의 udata에 전달하는 구조체.
 */
typedef struct s_ev_udata {
	std::string *response;
	int  byteWrote;
	int  byteLeft;
} t_ev_udata;

void setClientSockOpt(int client)
{


}

/**
 * @brief http 1.1 을위한 server socket에 대한 setsockopt
 *
 * @param serverfd
 */
void setServerSockOpt(int server)
{
  int keep_alive = 1;
  if (setsockopt(server, SOL_SOCKET, SO_KEEPALIVE, &keep_alive, sizeof(keep_alive)) < 0) {
    perror("setsockopt");
    exit(1);
  }

  // Disable Nagle algorithm
  // http1.1에서는  사용되지 않음.
  int nagle_off = 1;
  if (setsockopt(server, IPPROTO_TCP, TCP_NODELAY, &nagle_off, sizeof(nagle_off)) < 0) {
    perror("setsockopt");
    exit(1);
  }
}


void printEvent(struct kevent *kev)
{
	std::cout<<"=========현재 처리중인 이벤트========="<<std::endl;
	std::cout<<"ident:"<<kev->ident<<std::endl;
	std::cout<<"filter:"<<kev->filter<<std::endl;
	std::cout<<"flags:"<<kev->flags<<std::endl;
	std::cout<<"fflags:"<<kev->fflags<<std::endl;
	std::cout<<"data:"<<kev->data<<std::endl;
	std::cout<<"udata:"<<(kev->udata)<<std::endl;
	std::cout<<"======================================"<<std::endl<<std::endl;
}

char buffer[1024];
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


	//tcp keepalive
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


	/*
	 * server socket에 대해 nonblock socket임을 명시합니다.
	 * */
	fcntl(server_fd, F_SETFL, O_NONBLOCK);

	struct kevent kev;
	/*
	 * event를 등록하는 부분입니다.
	 * EV_SET을통해 등록할 이벤트를 정의합니다. 
	 * ident : server fd는 socket입니다. 
	 * ident가 socket의 fd라면
	 *
	 * EV_SET
	 * kevent를 초기화하는 함수
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
			std::cout << "\033[0m"; // set text color to green
			printEvent(events + i);

			if (events[i].flags & EV_EOF)
            {
				std::cout << "\033[30m"; // set text color to green
                printf("Client has disconnected");
                close(events[i].ident);
            }
			//server_fd에서 발생한 이벤트, 즉 새로운 client connection을 감지.
			//client fd를 read event를 등록한다.
			//client_fd가 read할 준비가 되면 event에 추가될것.
			else if (events[i].ident == server_fd) 
			{
				std::cout << "\033[31m"; // set text color to red
				std::cout<< "connection callback"<<std::endl;
				//connection 처리
				struct sockaddr_in client_address;
				socklen_t client_len = sizeof(client_address);
				int client_fd = accept(server_fd, (struct sockaddr *)&client_address, &client_len);
				std::cout<<"client fd: "<<client_fd<<std::endl;
				if (client_fd == -1) {
					std::cerr << "Failed to accept client connection\n";
					continue;
				}

				std::cout << "Client connected\n";
				std::cout<< client_address.sin_family<<std::endl;
				std::cout << "client port: "<<(client_address.sin_port)<<std::endl;
				std::cout << "client ipv4: "<<inet_ntoa(client_address.sin_addr) <<std::endl;


				//nonblock mode 명시된 server socket에서 받은 client socket도 자동으로 nonblock 설정된다.
				if (fcntl(client_fd, F_GETFL) & O_NONBLOCK)
					std::cout<<"in accept, client fd is nonblock"<<std::endl;


				//client socket을 읽기전용으로  kqueue에 등록
				EV_SET(&kev, client_fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
				if (kevent(kq_fd, &kev, 1, NULL, 0, NULL) == -1) {
					std::cerr << "Failed to register client socket with kqueue\n";
					continue;
				}
			}
			else if (events[i].filter == EVFILT_READ)
			{
				std::cout << "\033[32m"; // set text color to green
				std::cout<<"read callback"<<std::endl;
				// read callback
				//client로부터 들어온 데이터 처리
				int client_fd = events[i].ident;
				struct sockaddr_in client_address;
				socklen_t client_len = sizeof(client_address);

				int flags = fcntl(client_fd, F_GETFL);
				if (flags & O_NONBLOCK)
					std::cout<<"this fd is nonblocking"<<std::endl;
				ssize_t n = read(client_fd, buffer, sizeof(buffer));
				std::cout<<"n: "<<n<<std::endl;

				//read의 리턴값이 -1일때, errno가 EAGAIN(EWOULDBLOCK)인 경우, 아직 read될 준비가 되지 않은 것이므로, continue.
				//
				if (n == -1) 
				{
					std::cout<<"read returned -1, errno:"<<errno<<std::endl;
					if (errno == EAGAIN || errno == EWOULDBLOCK)
					{
						EV_SET(&kev, client_fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
						kevent(kq_fd, &kev, 1, NULL, 0, NULL);
						continue;
					}
					else
					{
						/* ? */
						std::cerr << "Failed to read from client socket, errno: "<<errno<<std::endl;
						close(events[i].ident);
					}
				}
				/*
				 * n이 0이라면, eof를 만난 것. kqueue에서 <client socket fd, EVFILT_READ>를 삭제
				 * */
				else if (n == 0) 
				{
					std::cout << "Client disconnected\n";
					//감시할 event에서 삭제.
					EV_SET(&kev, client_fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
					kevent(kq_fd, &kev, 1, NULL, 0, NULL);
					close(client_fd);
					std::cout<<"deleted"<<std::endl;
				}
				/*
				 * client socket에서 n byte만큼의 데이터를 읽어왔음.
				 * */
				else 
				{
					/*
					 * client request 출력
					 * */
					std::cout<<"=======CLIENT REQUEST======="<<std::endl;
					std::cout << "Received " << events[i].data << " bytes from client\n";
					for (int i = 0; i < n; i++)
						std::cout<<buffer[i];
					
					/*
					 * parsing http protocol.
					 * 1. 정적파일 제공
					 * 2. cgi 통해 동적파일 제공
					 * */
					/*
					 * chunked message 처리 => 
					 * if (the request not end) 
					 * {
					 * 		
					 * }
					 * */

					/* Example response */
					/* content length안지키면 client측에서 block되는 문제있음. 주의 */
					/* 동적할당된 response는 EV_WRITE에서 delete*/
					std::string *response = new std::string("HTTP/1.1 200 OK\r\nContent-Length: 5\r\n\r\naaaaa");
					

					t_ev_udata *udata = new t_ev_udata();
					memset(udata, 0, sizeof(t_ev_udata));
					udata->response = response;
					udata->byteLeft = response->length();

					/*
					 * client_fd에 response를 write하는 event를 kqueue에 추가.
					 * */
					EV_SET(&kev, client_fd, EVFILT_WRITE, EV_ADD, 0, 0, udata);
					if (kevent(kq_fd, &kev, 1, NULL, 0, NULL) == -1) {
						//write event 처리 실패
						std::cerr << "Failed to register client socket with kqueue\n";
					}
				}
			}
			else if (events[i].filter == EVFILT_WRITE)
			{
				std::cout << "\033[33m"; // set text color to green
				std::cout<<"write callback"<<std::endl;
				//write callback
				if (fcntl(events[i].ident, F_GETFL) & O_NONBLOCK)
					std::cout<<"this fd is nonblocking"<<std::endl;

				//write
				t_ev_udata *udata = static_cast<t_ev_udata *>(events[i].udata);
				int n = write(events[i].ident, udata->response->c_str() + udata->byteWrote, udata->byteLeft);

				if (n == -1)
				{
					if (errno == EAGAIN || errno == EWOULDBLOCK)
						continue;
					else
					{
						/* ? */
						std::cout<<"unknown err: "<<errno<<std::endl;
						continue;
					}
				}
				/*
				 * 모두 작성된 경우
				 * */
				else
				{
					//write event delete
					std::cout<<"all of data wrote"<<std::endl;

					delete static_cast<t_ev_udata *>(events[i].udata);
					delete static_cast<t_ev_udata *>(events[i].udata);
					EV_SET(&kev, events[i].ident, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
					kevent(kq_fd, &kev, 1, NULL, 0, NULL);
				}
			}
		}
	}
	return 0;
}
