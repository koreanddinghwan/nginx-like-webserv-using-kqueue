#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/_endian.h>
#include <sys/_types/_socklen_t.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

void error(const char *msg) {
  perror(msg);
  exit(1);
}

int main(int ac, char **av) {
  int sockfd;
  int newsockfd;
  int portno;
  socklen_t clilen;
  struct sockaddr_in serv_addr;
  struct sockaddr_in cli_addr;
  char buffer[256];

  int n;
  if (ac < 2) {
    fprintf(stderr, "Error, no port provided\n");
    exit(1);
  }

  // create socket
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
    error("ERROR ooooooopening socket");

  //clean address structure;
  bzero((char *) &serv_addr, sizeof(serv_addr));

  portno = atoi(av[1]);

//setup host_addr structure for use in bind call
//server byte order -> bigendian

  //set ipv4 
  serv_addr.sin_family = AF_INET;

  //automatically be filled with current host's IP address
  serv_addr.sin_addr.s_addr = INADDR_ANY;

  //convert short int for port must be converted into network
  serv_addr.sin_port = htons(portno);

  //bind passed fd, the addr_structure, the length of addr structure
  // This bind() call will bind  the socket to the current IP address on port, portno
  //unamed socket에 이름을 부여한다.
  if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
	  error("ERROR on binding");

  //sockfd가 incoming connection을 대기할 것이라는 것을 말한다.
  //2번째 인자는 queue의 길이, 처리되지 않은 connection은 pending 상태로 backlog 큐에존재한다.
  //accept 시스템콜에 의해서 처리될 것이다.
  listen(sockfd, 5);

  //accept 함수는 incoming connection을 허용한다.
  clilen = sizeof(cli_addr);

  //accept를 호출하면 backlog 큐에서 맨 앞에서부터 pending 상태인 connection을
  //추출하고, 새로운 소켓을 생성하고, 이 소켓에 대해 새로운 fd를 할당한다.
  //큐에 아무런 connection이 없고, socket이 논블로킹이 아니라면, accept는 새로운 connection이 수립될때까지 해당 라인에서 블락한다.
  newsockfd = accept(sockfd, (struct sockaddr*) &cli_addr, &clilen);


  if (newsockfd < 0)
	  error("ERROR on accept");

  printf("server got connection from %d port %d\n", (cli_addr.sin_addr.s_addr), ntohs(cli_addr.sin_port));


  


  //send() will send 13 byte of string to the new socket
  //need io Multiplexing in here
  send(newsockfd, "Hello, world!\n", 13, 6);

  bzero(buffer, 256);
  n = read(newsockfd, buffer, 255);
  if (n < 0)
	  error("ERROR reading from socket");

  printf("Here is the message: %s\n", buffer);
  close(newsockfd);
  close(sockfd);
  return 0;
}
