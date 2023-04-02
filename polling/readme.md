## select

- fd set에 대해 모두 순회하면서 각 fd에서 I/O 이벤트 여부 확인, fd많은경우 느림

## poll

- fd array에 대해 fd에 대한 I/O이벤트 여부 검사. select 보단 빠름

## epoll

- linux에서 주로 사용된다.
- 이벤트 발생시점에서만 호출, 해당 이벤트가 발생한 fd만 검사.
- fd 수와는 관계없이 일정한 속도로 동작.
- 많은 수의 fd를 처리할때 높은 성능, 대규모 서버, 네트워크 어플리케이션에 주로 사용.

## kqueue

- FreeBSD, macOS에서 사용된다.
- macOS의 커널은 대부분 kqueue를 사용해 다양한 이벤트를 처리한다.
- kevent 구조체를 사용해 이벤트를 등록하고, 이벤트가 발생하면 등록된 이벤트들을 검사한다.
- kqueue는 파일디스크립터 외에도 프로세스, timer, signal등 다양한 이벤트를 처리할 수 있다.

- webserv는 서버 어플리케이션, macos에서 동작하므로, kqueue을 사용하는 것이 옳다고 사려된다.

<br><br>

# socket

## socket

## setsockopt

- socket 옵션 값을 설정하는 시스템콜.

```cpp

#include <sys/types.h>
#include <sys/socket.h>

int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);
```

<br>

- sockfd: 옵션 값을 설정할 소켓의 파일 디스크립터입니다.
- level: 옵션 값을 설정할 프로토콜 레벨(level)을 지정합니다. 예를 들어, SOL_SOCKET은 일반 소켓 옵션을 지정하며, IPPROTO_TCP는 TCP 소켓 옵션을 지정합니다.
- optname: 설정할 옵션의 이름입니다.
- optval: 설정할 옵션의 값을 가리키는 포인터입니다.
- optlen: optval이 가리키는 값의 길이입니다.

<br>

### SOL_SOCKET vs IPPROTO_TCP

- 이 소켓 옵션이 어느 레벨에서 적용되는지를 의미한다.
- SOL_SOCKET: Internet layer의 socket option 설정 시 사용.
- IPPROTO_TCP: Transport layer의 socket option 설정 시 사용.

<br>

- optname이름, optval의 값으로 표현되는 option이 어느 internet layer에서 적용될 지 결정.
- 각 level에따라 적용될 수 있는 option이 다르다.

<br>

### options

- SOL_SOCKET 옵션
  - SO_ACCEPTCONN: 소켓이 수신 대기 상태인지 여부를 설정합니다.
  - SO_BROADCAST: 소켓이 브로드캐스트 메시지를 전송할 수 있는지 여부를 설정합니다.
  - SO_DEBUG: 소켓 디버깅을 사용할지 여부를 설정합니다.
  - SO_DONTROUTE: 소켓이 경로 탐색을 사용하지 않고 직접 목적지로 전송할지 여부를 설정합니다.
  - SO_ERROR: 소켓 오류를 검색할 때 사용합니다.
  - SO_KEEPALIVE: TCP 연결이 유지되는 동안 주기적으로 keepalive 패킷을 보낼지 여부를 설정합니다.
  - SO_LINGER: 소켓이 닫힐 때 보류 중인 데이터를 처리하는 방법을 설정합니다.
  - SO_RCVBUF: 소켓의 수신 버퍼 크기를 설정합니다.
  - SO_RCVLOWAT: read() 함수가 반환하기 위해 수신 대기 중인 데이터의 최소 크기를 설정합니다.
  - SO_RCVTIMEO: 소켓 수신 작업의 시간 초과 값을 설정합니다.
  - SO_REUSEADDR: 서버 소켓이 종료된 후에도 동일한 포트 번호를 재사용할 수 있도록 합니다.
  - SO_SNDBUF: 소켓의 송신 버퍼 크기를 설정합니다.
  - SO_SNDLOWAT: write() 함수가 반환하기 위해 송신 대기 중인 데이터의 최소 크기를 설정합니다.
  - SO_SNDTIMEO: 소켓 송신 작업의 시간 초과 값을 설정합니다.

<br>

- IPPROTO_TCP 옵션

  - TCP_NODELAY: Nagle 알고리즘을 비활성화하여 작은 데이터 패킷을 즉시 전송할 수 있도록 합니다.
  - TCP_MAXSEG: TCP 세그먼트 크기를 설정합니다.

- `이 option들 별로 사용되는 optval의 의미가 다르다.`
  - SO_REUSEPORT의 경우 optval이 1이면 활성화, 0이면 비활성화
  - SO_SNDBUF의 경우, 소켓 송신 버퍼 크기 지정의 역할.
