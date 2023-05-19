# include "HttpRequest.hpp"

int main()
{
	std::string sep1, sep2, chunked, basic;

	basic = "GET / HTTP/1.1\r\n"
          "Host: localhost:4000\r\n"
          "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8\r\n"
          "Accept-Encoding: gzip, deflate, br\r\n"
          "Accept-Language: ko-KR,ko;q=0.9,en-US;q=0.8,en;q=0.7\r\n"
          "Cookie: hblid=gupPzH3lCNmZhozG3m39N0GYB8Br6rH4; olfsk=olfsk957851537899227; connect.sid=s%3Adx8XBNTOX08FkVT8c2WsATIvIESlGujp.aH3cDWPLxXPV6pkFyI3AAAGi5zLTmmmxgPmgf%2FNOaJ4; key=eyJpc0xvZ2dlZEluIjp0cnVlLCJ1c2VybmFtZSI6IjExMTEifQ==\r\n"
          "Upgrade-Insecure-Requests: 1\r\n"
          "User-Agent: Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/67.0.3396.99 Safari/537.36\r\n\r\n";

	chunked = "GET / HTTP/1.1\r\n"
          "Host: localhost:4000\r\n"
          "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8\r\n"
          "Accept-Encoding: gzip, deflate, br\r\n"
          "Accept-Language: ko-KR,ko;q=0.9,en-US;q=0.8,en;q=0.7\r\n"
          "Cookie: hblid=gupPzH3lCNmZhozG3m39N0GYB8Br6rH4; olfsk=olfsk957851537899227; connect.sid=s%3Adx8XBNTOX08FkVT8c2WsATIvIESlGujp.aH3cDWPLxXPV6pkFyI3AAAGi5zLTmmmxgPmgf%2FNOaJ4; key=eyJpc0xvZ2dlZEluIjp0cnVlLCJ1c2VybmFtZSI6IjExMTEifQ==\r\n"
          "Upgrade-Insecure-Requests: 1\r\n"
          "User-Agent: Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/67.0.3396.99 Safari/537.36\r\n\r\n"
		   "Transfer-Encoding: chunked\r\n\r\n";

	sep1 = "GET / HTTP/1.1\r\n"
          "Host: localhost:4000\r\n"
          "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8\r\n"
          "Accep";
	
	sep2 = "POST / HTTP/1.1\r\n"
          "Host: localhost:4000\r\n"
          "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8\r\n"
          "Accept-Encoding: gzip, deflate, br\r\n"
          "Accept-Language: ko-KR,ko;q=0.9,en-US;q=0.8,en;q=0.7\r\n"
		  "Content-Length: 493\r\n"
          "Cookie: hblid=gupPzH3lCNmZhozG3m39N0GYB8Br6rH4; olfsk=olfsk957851537899227; connect.sid=s%3Adx8XBNTOX08FkVT8c2WsATIvIESlGujp.aH3cDWPLxXPV6pkFyI3AAAGi5zLTmmmxgPmgf%2FNOaJ4; key=eyJpc0xvZ2dlZEluIjp0cnVlLCJ1c2VybmFtZSI6IjExMTEifQ==\r\n"
          "Upgrade-Insecure-Requests: 1\r\n"
          "User-Agent: Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/67.0.3396.99 Safari/537.36\r\n\r\n"
		"{\n\t바디입니다\n바디고요\n바디라고합니다 사실 get에는 없지만 일단 확인 해보는겁니다...\n";


	std::cout << "sep1" << std::endl;
	HttpRequest req1(sep1);
	std::cout << "\n\nsep2" << std::endl;
	HttpRequest req2(sep2);
	std::cout << "\n\nbasic" << std::endl;
	HttpRequest req3(basic);
	std::cout << "\n\nchunked" << std::endl;
	HttpRequest req4(chunked);
	return (0);
}