#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

void error(const char *msg)
{
	perror(msg);
	exit(0);
}

int main(int ac, char **av)
{
	struct addrinfo hints, *res;
	int sockfd;
	int status;

	char buffer[256];

	if (ac < 3)
	{
		fprintf(stderr, "usage %s hostname port\n", av[0]);
		exit(1);
	}
	//SOCK_STREAM: TCP 프로토콜을 의미.
	//SOCK_DGRAM: UDP 프로토콜 의미
	//SOCK_RAW: 직접 IP프로토콜 조작하는 소켓유형.


	//setup hints for getaddrinfo
	//진짜 말 그대로 hint임. 설정안하면 시스템에서 알아서 선택함.
	//address info에서 어떤 종류의 connection을 할 것인지 의미.
	//ai_family: IPV4, IPV6등을 의미.
	//ai_socktype: 사용할 소켓의 유형, SOCK_STREAM, SOCK_DGRAM, SOCK_RAW등.
	//ai_protocol: socket으로 소켓 생성시 어떤 프로토콜 사용할지 결정.
	//aisocktype이 SOCK_STREAM이면서 ai_protocol이 TCP면 socket은 TCP소켓을 생성한다.
	//둘다 0이 될 수 있고, 시스템에서 알아서 선택도 한다.
	

	//setup hints for getaddrinfo
	//진짜 말 그대로 hint임. 설정안하면 시스템에서 알아서 선택함.
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_socktype = SOCK_STREAM;

	//host의 address information을 위의 hint를 기반으로 확인.
	//res에 결과값이 담긴다.
	if ((status = getaddrinfo(av[1], av[2], &hints, &res)) != 0) {
		fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
		exit(1);
	}

	//socket생성
	sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (sockfd == -1)
	{
		perror("ERROR on socket");
		exit(1);
	}


	//server 연결
	if (connect(sockfd, res->ai_addr, res->ai_addrlen) == -1){
		perror("EXIT, connection error");
		exit(1);
	}

	//server에 message전송
	const char *message = "GET /";
	int sendedByte = send(sockfd, message, strlen(message), 0);
	if (sendedByte == -1)
	{
		perror("Exit, send error");
		exit(1);
	}

	printf("send %d byte to %s:%s\n", sendedByte, av[1], av[2]);
	freeaddrinfo(res);
	close(sockfd);

	return 0;
}
