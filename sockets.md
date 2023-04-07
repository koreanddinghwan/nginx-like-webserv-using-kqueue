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

<br><br>
