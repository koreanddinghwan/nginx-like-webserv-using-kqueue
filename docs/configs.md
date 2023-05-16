# configuration

## server 

- server의 port와 host를 선택한다.
- apache에서는 virtualHost라는 용어를 사용하나, NGINX에서는 이러한 단어를 사용하지 않는다.
- NGINX는 Server Block의 server_name과 listen 지시자를 통해 tcp socket을 binding한다.

<br>

### server_name

- server_name 지시어가 host를 의미한다.
- 어떻게 사용되냐면, http protocol의 request header필드를 보면 Host가 있다.

<img width="348" alt="스크린샷 2023-05-14 20 13 30" src="https://github.com/koreanddinghwan/koreanddinghwan.github.io/assets/76278794/f5e7775f-6f49-4d99-a9a3-b23bffec4704">

- 서버가 요청을 받으면, 이 Host필드의 값과 server_name을 비교해서 일치하는 server_block으로 처리한다.

<br>

## default server

- default server는 `conf 파일에서 가장 상단의 server block`을 의미한다.
- 서브젝트에서는 Host필드가 아무 server block도 일치하지 않는다면, conf파일에서 가장 상단의 server에서 요청을 처리한다.

<br>

## default error pages

<img width="458" alt="스크린샷 2023-05-14 20 17 50" src="https://github.com/koreanddinghwan/koreanddinghwan.github.io/assets/76278794/3bd4eaa3-1212-4db2-8faf-761e3dc76e15">

[디폴트 에러페이지 설정](https://hyeonguj.github.io/2019/09/25/nginx-%EB%94%94%ED%8F%B4%ED%8A%B8-%EC%97%90%EB%9F%AC%ED%8E%98%EC%9D%B4%EC%A7%80-%EC%84%A4%EC%A0%95%ED%95%98%EA%B8%B0/)


- location block은 
	- prefix match : `location uri`
	- exact match : `location = uri`
	- regex match : `location ~ uri`
	- preferential prefix match : `location ^~ uri`

이 있고, 우선순위는 `exact > preferential > regex > prefix`이다.


<br>

## client max body size

- `http, server, location` block에서 사용합니다.
- client_max_body_size는 client request body의 크기를 제한합니다.
- 만약 request가 이 값을 초과하면 413 status를 리턴합니다.
- 0으로 설정하면 client request body size를 확인하지 않습니다.

<br>

## accepted http method 설정


```
location /redirecturl {
	limit_except GET POST HEAD {
		deny all
	}
	autoindex off;
	return 301 testurl
}
```

- 위 설정을 보면 `limit_except`키워드가 있는데, 해당 location에서 limit_except에서 나열된 GET, POST,HEAD를 제외하곤 나머지 http request를 deny한다는 설정.
- nginx docs[limlit_except](https://nginx.org/en/docs/http/ngx_http_core_module.html#limit_except)에선 GET method의 allow는 HEAD method도 allow한다고 명시하고있습니다.

<br>


## http redirection

- 마찬가지로 위에서 설명한 설정을 보면, 


```
location /redirecturl {
	limit_except GET POST HEAD {
		deny all
	}
	autoindex off;
	return 301 testurl
}
```

- location block의 uri로 `/redirecturl`와 함께 해당 location blocK이 `return 301 testurl`이 설정된 것을 확인할 수 있습니다.  
- client가 `/retirecturl`로 요청을 보내면, 해당 요청을 testurl로 `redirection`하고, 301을 리턴합니다.



<br>

## define directory or a file from where the file should be searched

- location block의 location directives 뒤의 url은 사용자의 요청을 `prefix/exact/regex`등의 rule을 통해 매치시키며, 사용자가 어떤 자원을 요청하는지 식별하게됩니다.
- 그래서 `root`라는 directive가 필요한데, 사용자가 요청한 자원에 대해 서버의 어느 폴더에서 자원을 찾아서 리턴해줄지 결정합니다.

```
location /kapouet {
	root /tmp/www
}
```

- 사용자가 `servername/kapouet/a/b/c`로 요청을 보내면, 서버는 `/tmp/www/a/b/c`의 자원을 리턴합니다.


<br>

## default file to answer if the request is directory

- nginx의 [index config](https://nginx.org/en/docs/http/ngx_http_index_module.html#index)로 설정.

- /로 끝나는 사용자의 request를 처리합니다.
- index file들을 정의하고, 순서대로 파일을 보냅니다.
- 사용자가 `localhost:8080` url로 요청을 보내면 브라우저는 이를 `/`자원에 대한 요청으로 변환합니다.

<img width="1156" alt="스크린샷 2023-05-16 17 47 06" src="https://github.com/koreanddinghwan/nginx-like-webserv-using-kqueue/assets/76278794/8651904b-1492-40ea-86af-e709be0350ad">

- 이 index 모듈에서 사용자의 request를 처리하지 못하면, request는 autoindex_module로 전달됩니다.


### internal redirection

```
location = / {
    index index.html;
}

location / {
    ...
}
```

- 사용자의 /요청은 `exact match rule`에 의해 첫 번째 location block에서 처리되며,
- index directive 뒤에 있는 자원을 차례대로 확인해서 있는 자원으로 요청을 변환하고,
- 이 예시에서 /index.html로 변환된 자원은, 2번째 location block에서 처리되어 /index.html 파일을 전달하게됩니다.

<br>

## turn on/off directory listing

- [autoindex](https://nginx.org/en/docs/http/ngx_http_autoindex_module.html)

- /로 끝나는 client의 request에 대해 directory listing을 제공합니다.
- `request는 먼저 ngx_http_auto_index_module`을 통과하며, index file을 찾지 못하면 `ngx_http_index_module`을 통과합니다.











