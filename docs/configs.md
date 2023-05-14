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

## 
