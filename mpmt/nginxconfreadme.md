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

# static content

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

# fastcgi proxy

## cgi

- 과거에 웹 서버 상에서 사용자 프로그램을 동작시키기위한 조합.
- 서버프로그램과 외부 프로그램(DB등) 사이의 연계를통해 `동적인 페이지를 생성`해 Web Server에 보낸다.
- 언어에 구애받지 않음.
- `사용자 요청마다 프로세스를 생성하기때문에 서버에 부하`

<br>

## fast-cgi

- 웹서버의 프로세스로서 인터프리터를 상주시켜, CGI로부터 프로그램을 호출해 부하를 줄인다.
- `1개의 프로세스를 상주시켜 부하를 줄인다.`
- 아,,,물론 단일프로세스라서 요청처리하는동안 요청을 blocking하는 문제가 있다.
- 그래서 보통은 이벤트기반 논블로킹 I/O를 지원하는 REACYPHP랑 같이 쓴다.

<br>

## proxy

- proxy는 `클라이언트와 서버 사이의 중개자`이다.
- nginx는 기본적으로 프록시를 지원한다.

### reverse proxy

- nginx에서 클라이언트와 웹서버사이에 위치하며, 클라이언트의 요청을 적절한 웹서버로 전송한다.

  - `(internet)client->nginx->webserver(회사)`

- http proxy서버(흔히 스프링, 장고, 노드 등) 혹은 non-http proxy server(FastCGI, uwsgi, SCGI, memcached 프로토콜을 사용하는)에 client의 request를 전달한다.

- `보안`: nginx의 서버 주소만 노출하고, 내부 실제 요청을 처리하는 서버의 주소를 숨길 수 있다.
- `로드밸런싱` : 요청을 처리하는 서버를 여러대 두어 로드밸런싱을 할 수 있다.

<br>

### (번외)forward proxy

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

[](https://docs.nginx.com/nginx/admin-guide/web-server/reverse-proxy/)
[](https://nginx.org/en/docs/beginners_guide.html#control)
