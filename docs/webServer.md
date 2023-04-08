# `NGINX: Advanced Load Balancer, Web Server, & Reverse Proxy`

nginx을 검색하면 이런 결과가 나옵니다.  

<img width="760" alt="Screen Shot 2023-04-08 at 3 58 01 PM" src="https://user-images.githubusercontent.com/76278794/230708027-fb03a652-5a01-419a-9fda-eabfd84eb6f1.png">  

- 순서대로하기보다는 웹서버의 개념, WAS의 개념, 그리고 리버스 프록시, 로드밸런서 이 순서대로 설명하는게 좋을 것 같습니다.



# Web Server

![web-servers](https://user-images.githubusercontent.com/76278794/230708348-874a9e52-77b0-4c0e-978f-62b5b6fb3e7f.png)
- 정의: `웹 브라우저`와 같은 클라이언트로부터 HTTP 요청을 받아들이고, HTML 문서와 같은 웹 페이지를 반환하는 프로그램

웹 서버는 html, css, js, image 등 `정적 콘텐츠`의 전달을 목적으로합니다.  
1. 브라우저가 HTTP를 통해 파일을 요청
2. 웹서버가 요청을 받으면, 해당 콘텐츠를 찾아서 HTTP를 통해 브라우저로 다시 전달.

<br>


- 그런데, 문제는 사용자의 요청을 통해 데이터베이스에 접근해 데이터를 변경하고, 저장하고, 삭제하는 등 비즈니스 로직을 처리해야하는 경우가 있습니다.
- 이러한 동적컨텐츠의 전달은 웹서버가 `WAS`를 프록시해서 WAS에서 처리한 결과를 웹서버가 다시 클라이언트에 전달하게됩니다.


<br><br>

# WebApplication Server

- 정의: `동적컨텐츠 전달`을 목표로 일반적으로 데이터베이스 서버와 같이 수행되는 서버.

이 WAS는 한국어권에서 사용되는 말이며, 영어권에서는 일반적으로 `application server`로 불립니다.   


그런데, 우리가 흔히 공부하는 Spring, NestJS, express, django등 다양한 프레임워크는 웹 서버, WAS, 둘 중 뭐에 속할까요?  

`제 생각에는 사용하기에따라 두개 다로 사용할 수 있다.`입니다.  
가령, express 프레임워크를 사용하면서 이 서버가 정적컨텐츠, 동적컨텐츠 모두를 전달할수도 있고,  
프론트엔드 서버처럼 정적컨텐츠의 전달만을, 혹은 데이터베이스 I/O를 통한 동적컨텐츠의 전달만을 목표로 할 수도 있습니다.  

<br>

- Web Server, Web Application Server, Reverse proxy server, Load Balancer 모두 역할을 의미하는 것이며, 프로그래머가 서비스를 설계하기 나름으로 nginx나 express, django 등에 어떤 역할을 부여하느냐의 개념으로 이해할 수 있습니다.  


<br>

# Reverse Proxy

- nginx는 외부 인터넷의 클라이언트의 요청을 다른 서버에 전달하는 리버스 프록시 서버 역할을 할 수 있습니다.
- nginx docs에 proxy에 대한 설명이 나와있습니다.

![리버스프록시, 포워드 프록시](https://github.com/koreanddinghwan/webserv_tutorial/blob/main/docs/nginx.md#proxy)  

<br>

# 로드밸런서

- nginx에 전달되는 HTTP 트래픽을 웹서버 혹은 WAS에 부하를 분산할 수 있습니다.

```
http {
    upstream backend {
        server backend1.example.com;
        server backend2.example.com;
        server 192.0.0.1 backup;
    }
    
    server {
        location / {
            proxy_pass http://backend;
        }
    }
}
```

- upstream block 로드밸런싱할 그룹을 정의합니다.
- server block에서http://backend는 / 요청에 대해서 http://backend 가 있는데, HTTP request를 backend server group으로 프록시하고 있습니다. 
- 이 로드밸런싱은 `라운드로빈` 알고리즘을 사용합니다.

<br>

- nginx는 지속적으로 `health check`를 통해 프록시된 서버 중 요청을 받지 못하는 서버를 회피하고, 복구된 서버를 다시 그룹에 넣는 일을 합니다.
