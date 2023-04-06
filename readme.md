# 설계

[draw.io](https://drive.google.com/file/d/1vH0PNeXKEOKTo_2sLT3yukgT2I_hUp2z/view?usp=sharing)


# CGI 아키텍처 다이어그램

<img width="361" alt="스크린샷 2023-04-01 20 28 17" src="https://user-images.githubusercontent.com/76278794/229286069-17e194f8-b556-4d35-bc87-120abb7282e1.png">

<br>

# CGI란?

- Common Gateway Interface는 web server와 custom script 간 정보 교환 방식을 정의하는 표준이다.
- CGI spec는 NCSA가 유지보수하며 NCSA가 CGI를 정의한다.
- CGI(Common Gateway Interface)는 외부 게이트웨이 프로그램이 HTTP 서버와 같은 정보 서버와 인터페이스하기 위한 표준입니다.

# Web server 설정

- CGI 프로그래밍을 계속하기 전에 웹 서버가 CGI를 지원하고 CGI 프로그램을 처리하도록 구성되어 있는지 확인하십시오.
- HTTP 서버에서 실행할 모든 CGI 프로그램은 사전 구성된 디렉토리에 보관됩니다.
- 이 디렉터리는 CGI 디렉터리라고 하며 관례에 따라 /var/www/cgi-bin으로 이름이 지정됩니다.
- CGI 파일은 C++ 실행 파일이지만 .cgi로 확장된다.
- 기본적으로 Apache Web 서버는 /var/www/cgi-bin에서 CGI 프로그램을 실행하도록 구성되어 있습니다.
- CGI 스크립트를 실행할 다른 디렉터리를 지정하려면 httpd.conf 파일에서 다음 섹션을 수정할 수 있습니다

```cpp
<Directory "/var/www/cgi-bin">
   AllowOverride None
   Options ExecCGI
   Order allow,deny
   Allow from all
</Directory>

<Directory "/var/www/cgi-bin">
   Options All
</Directory>
```

# CGI by CPP

- c++로 작성된 이런 프로그램도 CGI가 될 수 있습니다.

```cpp
#include <iostream>
using namespace std;

int main () {
   cout << "Content-type:text/html\r\n\r\n";
   cout << "<html>\n";
   cout << "<head>\n";
   cout << "<title>Hello World - First CGI Program</title>\n";
   cout << "</head>\n";
   cout << "<body>\n";
   cout << "<h2>Hello World! This is my first CGI program</h2>\n";
   cout << "</body>\n";
   cout << "</html>\n";

   return 0;
}
```

<br><br>

# Nginx

- Nginx는 기본적으로 다중프로세스/스레드 모델이다.

<br>

## 프로세스

- Nginx는 1개의 마스터 프로세스와 여러개의 워커 프로세스로 구성된다.
- 마스터 프로세스는 주소 환경 설정 파일의 로드와 같은 초기화작업을 수행한다.
- 실제 클라이언트 요청 처리 작업은 워커프로세스에서 수행된다.
- 기본값으로 CPU코어의 개수이고, 수동으로 설정할 수 있다.
- `worker_processes`로 설정한다. 

<br>

## 스레드

- nginx는 기본적으로 각 워커 프로세스가 1개의 스레드를 사용해 요청을 처리한다.
- 하지만, 이 각 워커프로세스하나로만 요청을 처리하는 것은 시간이 오래걸린다.
- `read 시스템콜, 리눅스의 sendfile, 캐시파일같은 임시파일 작성을위한 리눅스의 aio_write`등을 사용할때, worker thread에 이러한 작업을 offload한다.
- 특히, `read, 와 같이 시간이 오래걸리는 작업`은 Thread Pool에 있는 워커스레드에서 처리한다.
  - Memory에 Cache되어있지 않은 경우에 시간이 오래걸린다.
  - Async model을 선택할수도있지만 FreeBSD에서만 가능허거나 리눅스에서는 모든 파일 액세스가 메모리 캐시를 바이패스하는 문제가 있다.
- `그러나, 대부분의 file i/o 작업은 직접적으로 하드드라이브에서 가져오는 것이 아닌, 운영체제가 충분한 RAM이 있는경우, 페이지 캐시를 한다.`
- `이 경우, 스레드풀을 사용하는 것이 오버헤드가 더 있다고 한다.`
- 스레드풀을 사용하는 상황은 `NGINX 기반 스트리밍 미디어 서버`인 경우와 같은 로드가 많고, 운영체제의 가상메모리 캐시에 맞지 않은 양의 컨텐츠인 경우에 스레드풀로 오프로드가 더 많은 효율을 발휘한다.
- 

[스레드풀 관련 공식문](https://www.nginx.com/blog/thread-pools-boost-performance-9x/)

<br>


# config for nginx

nginx는 구성 파일에 지정된 명령에 의해 제어되는 모듈로 구성.

`directive`는 `simple` 디렉티브와 `block` 디렉티브로 나뉩니다.

simple: 이름과 매개 변수로 구성되며 공백으로 구분되고 세미콜론(;)으로 끝납니다.

block: simple 디렉티브와 구조가 같지만 세미콜론 대신 대괄호({ 및 })로 둘러싸인 추가 명령 집합으로 끝납니다.  
대괄호 안에 다른 지시어를 포함할 수 있는 경우 context라고 합니다(예: event, http, server, location).

<br>

컨텍스트 외부의 구성 파일에 배치된 지시사항은 기본 컨텍스트에 있는 것으로 간주됩니다.
`event 및 http 지시어는 기본 context`, `server는 http`, `location은 server`에 있습니다.
\# 기호 뒤에 있는 줄의 나머지 부분은 주석으로 간주됩니다.

<br><br>

## static content

`html or image`와 같은 정적파일 전달.  
http 블록 안의 server 블록안에 location블록에 할당한다.

```conf
http {
	server {
		listen 8080;
		location / {
			root /data/www;
		}

		location /images {
			root /data;
		}
	}
}
```

- server는 longest prefix matching을 통해 가장 긴 주소와 일치하는 location block을 사용.

- `/images`로 시작하지 않는 모든 URI는 모두 `/data/www`에 매핑된다.
- `http://localhost/some/example.html` -> `/data/www/some/exaple.html`로 매핑

<br>

## fastcgi proxy

### cgi

- 과거에 웹 서버 상에서 사용자 프로그램을 동작시키기위한 조합.
- 서버프로그램과 외부 프로그램(DB등) 사이의 연계를통해 `동적인 페이지를 생성`해 Web Server에 보낸다.
- 언어에 구애받지 않음.
- `사용자 요청마다 프로세스를 생성하기때문에 서버에 부하`

<br>

### fast-cgi

- 웹서버의 프로세스로서 인터프리터를 상주시켜, CGI로부터 프로그램을 호출해 부하를 줄인다.
- `1개의 프로세스를 상주시켜 부하를 줄인다.`
- 아,,,물론 단일프로세스라서 요청처리하는동안 요청을 blocking하는 문제가 있다.
- 그래서 보통은 이벤트기반 논블로킹 I/O를 지원하는 REACYPHP랑 같이 쓴다.

<br>

### proxy

- proxy는 `클라이언트와 서버 사이의 중개자`이다.
- nginx는 기본적으로 프록시를 지원한다.

1. reverse proxy

- nginx에서 클라이언트와 웹서버사이에 위치하며, 클라이언트의 요청을 적절한 웹서버로 전송한다.

  - `(internet)client->nginx->webserver(회사)`

- http proxy서버(흔히 스프링, 장고, 노드 등) 혹은 non-http proxy server(FastCGI, uwsgi, SCGI, memcached 프로토콜을 사용하는)에 client의 request를 전달한다.

- `보안`: nginx의 서버 주소만 노출하고, 내부 실제 요청을 처리하는 서버의 주소를 숨길 수 있다.
- `로드밸런싱` : 요청을 처리하는 서버를 여러대 두어 로드밸런싱을 할 수 있다.

<br>

2. (번외)forward proxy

- 내부망에서 발생한 client의 요청을 internet에 전달한다.
- 회사에서 인터넷을 사용할때 보통 이렇게 사용된다.(누군가는 다 알고 있다.)

<br>

### fastcgi proxy 예시

```conf
server {
    location / {
        fastcgi_pass  localhost:9000;
        fastcgi_param SCRIPT_FILENAME $document_root$fastcgi_script_name;
        fastcgi_param QUERY_STRING    $query_string;
    }

    location ~ \.(gif|jpg|png)$ {
        root /data/images;
    }
}
```

- fastcgi_pass 지시어로 fastcgi 프로토콜을 사용해 fastcgi 서버로 요청을 프록시한다.
- fastcgi에 넘길 파라미터를 미리 정의된 파라미터로 던지고있다.

[nginx 리버스프록시](https://docs.nginx.com/nginx/admin-guide/web-server/reverse-proxy/)
[nginx 비기너 가이드](https://nginx.org/en/docs/beginners_guide.html#control)


# worker_connections

[what are the worker_connections](https://linuxhint.com/what-are-worker-connections-nginx/)
