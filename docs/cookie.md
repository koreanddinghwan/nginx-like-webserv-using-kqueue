# SESSION, Cookie

HTTP1.0 → 1996년 RFC1945

HTTP1.1 → 1999년 RFC 2616

Cookie(HTTP state management mechanism) ⇒ 2000년 RFC 2965

HTTP2.0 → 2015년 RFC7540

<br>

## HTTP, stateless?

HTTP는 기본적으로 stateless하다고 합니다.

- 각각의 request과 response는 다른 request와 response에 독립적이기때문이다.
- 서버는 특정 client의 요청에 대해 정보를 유지하지 않으며, client도 이번 요청에 대한 정보를 유지할 필요가 없기 때문이다.

- HTTP/1 은 본질적으로 stateless를 추구했지만, HTTP/2의 대다수의 구성요소는 stateful하다. 그래서 HTTP/2는 상태비저장을 포기하며 HTTP/2 표준의 핵심에 정의된다.

[Why is it said that "HTTP is a stateless protocol"?](https://stackoverflow.com/questions/13200152/why-is-it-said-that-http-is-a-stateless-protocol)

rfc7540, http2에 명시된  cookie header field. 
![스크린샷 2023-01-18 01 45 56](https://user-images.githubusercontent.com/76278794/230706828-04dcba64-102e-4d60-9ff5-c40e5249c82e.png)

상동, stateless라는 언급을 찾아볼 수 없다.
![스크린샷 2023-01-18 01 49 00](https://user-images.githubusercontent.com/76278794/230706832-b4e3cd6c-c0a6-4415-9c4b-6d4b25fbb22a.png)




rfc2616, http/1.1에는 cookie 관련 명세가 없다.
![스크린샷 2023-01-18 01 48 12](https://user-images.githubusercontent.com/76278794/230706833-88059379-f55e-4737-b505-139e4f7cc32a.png)


![스크린샷 2023-01-18 01 49 23](https://user-images.githubusercontent.com/76278794/230706875-142fe3ec-40b0-4feb-a84f-aa1d79a95a71.png)
상동, stateless로 HTTP를 정의하고있다.

HTTP/2로 stateless한 프로토콜을 사용할수도 있지만, 현대 어플리케이션에서 비효율적이다.

<br><br>

## stateful한 어플리케이션 구현

HTTP의 본질은 stateless하니 이 문제를 해결하기 위해(statefull하기위해) Cookie를 사용해  Session고 Token방식이 존재한다.
- Cookie는 세션과 토큰방식의 statefull을 유지하기 위한 수단이다.

### 1. Cookie

Cookie는 RFC6265에 HTTP State Management Mechanism으로 소개된다.

[RFC 6265: HTTP State Management Mechanism](https://www.rfc-editor.org/rfc/rfc6265)

Set-Cookie header field가 HTTP server가 HTTP user(client)의 state를 저장할 수 있도록 사용되며, server가 stateless한 HTTP protocol로 session을 유지할 수 있도록한다.(2011년에는 HTTP/2가 없던 시절이라서 HTTP/1 으로 HTTP를 소개한다)

RFC에서 소개하는 절차는 다음과 같다.

1. To store state, the origin server includes a Set-Cookie header in an HTTP response.
    - state를 저장하기위해 server는 HTTP response에 Set-Cookie header를 포함한다.
2. In subsequent requests, the user agent returns a Cookie request header to the origin server. 
    - 후속 요청에 대해 client는 server에 Cookie request header를 전달한다.
3. The Cookie header contains cookies the user agent received in previous Set-Cookie headers.
    - Cookie header는 client가 이전에 받은 Set-Cookie header의 cookie이다.
4. The origin server is free to ignore the Cookie header or use its contents for an application-defined purpose.
    - server는 어플리케이션 목적에따라 Cookie header를 무시하거나 사용할 수 있다.

```
In subsequent requests, the user agent returns a
   Cookie request header to the origin server.  The Cookie header
   contains cookies the user agent received in previous Set-Cookie
   headers.  The origin server is free to ignore the Cookie header or
   use its contents for an application-defined purpose.
```

- server는 어느 response든 간에 Set-Cookie response header를 보낼 수 있으나, client는 100-level status code를 가진 response의 Set-Cookie header는 무시할 수 있다. 하지만 다른 status code에는(400, 500 level 을 포함해)모두 처리를 해줘야한다.
- server는 단일 response에 여러 Set-Cookie header field를 포함할 수 있으나, folding(병합)은 금지한다.

Cookie들은 /Users/person/Library/ApplicationSupport/Google/Chrome/Profile/COokies에 SQLite format 3 로 저장되어있다.

![스크린샷 2023-01-18 04 47 21](https://user-images.githubusercontent.com/76278794/230706893-955c8a69-2bc8-46d8-9d2d-76a008f0b8ab.png)
![스크린샷 2023-01-18 04 48 38](https://user-images.githubusercontent.com/76278794/230706896-20ac8a40-3a29-4d11-8d8e-a74ec6752037.png)


어디에 접속했었는지, 마지막 접속시간, http only 등등 정보가 담여있는 것을 확인할 수 있다.

<br>

### 2. Session

사용자의 정보 모두를 Cookie에 저장할수도 있지만, 이 방식은 Cookie header가 점점 무거워지는 경향이 있다. 

Cookie가 물리적으로 클라이언트측에 저장된다면, session은 서버측에서 저장한다.

물론, 여기서도 사용자를 인식하는 Session ID를 전송해 저장할 Cookie는 필요하다. session은 Cookie를 사용하는 방식 중 하나라고 생각하면 편하다.

 session ID를 발급해 session ID를 키값으로 value 값에 사용자 정보를 저장해 서버측에 저장한다.

이런 Session 방식은 Cookie header를 줄일 수 있는 큰 장점이 있다.

Session을 사용하는 방법으로는 

1. server memory
2. database
3. file storage
4. redis 같은 외부 session storage

같은 것들이 있다.

session을 사용하는 절차로는

1. client가 server에 최초 request
2. server에서 client의 Cookie에 session ID가 없는 것을 확인, 발급.
    - express 기준으로 express-session이라는 middleware에서 모든 request에 대해 확인합니다.
3. 자신의 session storage에 저장
4. 발급한 SID를 Cookie에 담아서 전달
5. ….
6. 이후 request에 대해서 session ID있는 것을 확인, request에서 session 객체를 사용할 수 있게됨, 이것으로 식별


<br>

### 3. Token

세션방식이 클라이언트의 요청 헤더의 sid 서버에서 키값으로 사용해 클라이언트를 식별하는 것이라면,  
토큰방식, 흔히 JWT를 통해 사용자를 식별하는 방식은 이 토큰 자체에 정보가 들어가 있다는 점이다.  

- 세션방식과 비교해 데이터베이스 등에서 사용자를 찾을 필요가 없기때문에 Search작업의 부하가 적다는 장점.
- 암호화-비암호화 과정에서 CPU연산이 필요하다는 단점
이 존재한다.

<br>

- 이 JWT토큰은 서버가 발급해주고, 쿠키의 만료기간 전까지 계속 req-res에 달려있게된다.
- 네트워크상에서 계속 통과하기때문에, 민감한 정보보다는 사용자 식별 정보만 담는 것이 원칙이다.

<br>

- 그리고 JWT의 가장 큰 맹점은 세션방식보다 `신뢰성이 떨어지기(사용자측에서 변조할 수 있기때문에)`때문에 서버측에서는 JWT토큰의 SECRET값을 잘 관리해야한다. 절대로 노출되면 안된다.
- 서버측에서 필요한 정보를 JWT 토큰에 담을때, 암호화 방식 및 SECRET값을 통해 서명하게되는데, 서명된 토큰은 사용자측에서 변조하면 서버측에서 decrpyt 할 때 변조된 토큰인지 확인 할 수 있다.


<br><br>


## 뭐가 좋은가?

- 뭐가 좋은지에 대한 정확한 답은 없다고 생각한다.
- 세션방식을 사용하게되면, 서버측에서 사용자를 직접 쳐낼 수 있다는 장점이 있다.
- 토큰방식을 사용하면 관리할 데이터베이스가 하나 줄어들고, 유연하게 토큰을 사용할 수 있고, 세션방식보다 I/O처리가 적다는 장점이 있다.

<br>

- 토큰방식의 경우, access token, refresh token을 사용한 로그인 유지를 계속할 수 있게 한다는 방법도 존재한다.
	- 토큰 자체가 데이터를 담고있고, 네트워크 상에 계속노출되며, 클라이언트 컴퓨터에 저장되어있다는 보안상의 취약점을 개선한다.
	- access token은 5분단위로, refresh token은 60일단위로 만료시키면서 그때그때 새로 발급해준다.
	- 해당계정에 로그인한 유저가 어떤 IP에서 마지막으로 접속했는지, 그리고 서버측에서 해당 유저의 refresh token을 revoke시키고 다시 로그인할 수 있게 한다는 기능도 만들 수 있다.(인스타, 네이버, 넥슨 등 보안에 신경쓰는 어플리케이션을 사용하다보면 로그인유지 기능도 있으면서 어떤 IP에서 새로 로그인했는지, 로그인한 유저를 로그아웃시킬지말지 등을 결정할 수 있다.)











