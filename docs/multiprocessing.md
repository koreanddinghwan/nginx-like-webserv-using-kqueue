# 멀티 프로세싱의 구현

- 마스터 프로세스: 서버 설정, 스레드풀 설정 등 수행
- 워커 프로세스: 사용자의 요청처리
- 마스터 <-> 워커 간 통신매체가 필요한데, Unix Domain Socket을 사용한다.
- kqueue와 같은 non-blocking i/o는 프로세스마다 할당해줘야한다.

## Unix Domain Socket

```cpp
int ipc_socket = socket(AF_UNIX, SOCK_STREAM, 0);
struct sockaddr_un server_unix_addr;
memset(&server_unix_addr, 0, sizeof(server_unix_addr));
server_unix_addr.sun_family = AF_UNIX;
strcpy(server_unix_addr.sun_path, "/var/run/nginx.sock");
bind(ipc_socket, (struct sockaddr*)&server_unix_addr, sizeof(server_unix_addr));
listen(ipc_socket, SOMAXCONN);
```

- Pipe는 단방향 통신만 가능하다. 즉, 파이프는 일방적으로 데이터가 흐르는 방향이 정해져 있다.
- 반면, Unix Domain Socket은 양방향 통신이 가능하다. 따라서 Unix Domain Socket을 사용하면 프로세스 간에 양방향 통신을 할 수 있으며, 데이터 흐름이 양쪽 방향으로 이루어진다.

<br>

- 또한, Unix Domain Socket은 TCP/IP 소켓과 같이 소켓 API를 사용한다. 따라서, 다른 네트워크 프로그래밍과 유사한 방식으로 프로세스 간 통신을 구현할 수 있다.
- 반면, Pipe는 파일 디스크립터를 사용하여 통신하며, 이를 통해 파일의 입출력과 같은 방식으로 프로세스 간 데이터를 전송한다.

<br>

- 마지막으로, Unix Domain Socket은 파일 시스템의 일부이므로, 파일 시스템에 기반한 다른 IPC 방식과 마찬가지로, 여러 프로세스 간에 공유될 수 있다.
- 따라서, Unix Domain Socket을 사용하면 IPC를 위한 파일과 같은 형태로 데이터를 전달할 수 있다.

<br>

- 따라서, Pipe와 Unix Domain Socket은 각각의 특징을 가지고 있으며, 사용 목적에 따라 적합한 IPC 메커니즘을 선택해야 한다.

# Nginx 아키텍처 참고

[nginx아키텍처 분석](https://cyuu.tistory.com/172)
