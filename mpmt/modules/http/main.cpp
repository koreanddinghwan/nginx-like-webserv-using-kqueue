#include "HttpreqHandler.hpp"

int main()
{
	std::string sep1, sep2, sep3, 
	chunked, 
	basic_post, basic_get, 
	post,
	c1, c2, c3, c4, c5,
	cookie;

	//basic = "GET / HTTP/1.1\r\n"
    //      "Host: localhost:4000\r\n"
    //      "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8\r\n"
    //      "Accept-Encoding: gzip, deflate, br\r\n"
    //      "Accept-Language: ko-KR,ko;q=0.9,en-US;q=0.8,en;q=0.7\r\n"
    //      "Cookie: hblid=gupPzH3lCNmZhozG3m39N0GYB8Br6rH4; olfsk=olfsk957851537899227; connect.sid=s%3Adx8XBNTOX08FkVT8c2WsATIvIESlGujp.aH3cDWPLxXPV6pkFyI3AAAGi5zLTmmmxgPmgf%2FNOaJ4; key=eyJpc0xvZ2dlZEluIjp0cnVlLCJ1c2VybmFtZSI6IjExMTEifQ==\r\n"
    //      "Upgrade-Insecure-Requests: 1\r\n"
    //      "User-Agent: Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/67.0.3396.99 Safari/537.36\r\n\r\n";

	//chunked = "GET / HTTP/1.1\r\n"
    //      "Host: localhost:4000\r\n"
    //      "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8\r\n"
    //      "Accept-Encoding: gzip, deflate, br\r\n"
    //      "Accept-Language: ko-KR,ko;q=0.9,en-US;q=0.8,en;q=0.7\r\n"
    //      "Cookie: hblid=gupPzH3lCNmZhozG3m39N0GYB8Br6rH4; olfsk=olfsk957851537899227; connect.sid=s%3Adx8XBNTOX08FkVT8c2WsATIvIESlGujp.aH3cDWPLxXPV6pkFyI3AAAGi5zLTmmmxgPmgf%2FNOaJ4; key=eyJpc0xvZ2dlZEluIjp0cnVlLCJ1c2VybmFtZSI6IjExMTEifQ==\r\n"
    //      "Upgrade-Insecure-Requests: 1\r\n"
    //      "User-Agent: Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/67.0.3396.99 Safari/537.36\r\n\r\n"
	//	   "Transfer-Encoding: chunked\r\n\r\n";

	//sep1 = "GET / HTTP/1.1\r\n"
    //      "Host: localhost:4000\r\n"
    //      "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8\r\n"
    //      "Accep";
	
	//sep2 = "POST / HTTP/1.1\r\n"
    //      "Host: localhost:4000\r\n"
    //      "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8\r\n"
    //      "Accept-Encoding: gzip, deflate, br\r\n"
    //      "Accept-Language: ko-KR,ko;q=0.9,en-US;q=0.8,en;q=0.7\r\n"
	//	  "Content-Length: 493\r\n"
    //      "Cookie: hblid=gupPzH3lCNmZhozG3m39N0GYB8Br6rH4; olfsk=olfsk957851537899227; connect.sid=s%3Adx8XBNTOX08FkVT8c2WsATIvIESlGujp.aH3cDWPLxXPV6pkFyI3AAAGi5zLTmmmxgPmgf%2FNOaJ4; key=eyJpc0xvZ2dlZEluIjp0cnVlLCJ1c2VybmFtZSI6IjExMTEifQ==\r\n"
    //      "Upgrade-Insecure-Requests: 1\r\n"
    //      "User-Agent: Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/67.0.3396.99 Safari/537.36\r\n\r\n"
	//	"{\n\t바디입니다\n바디고요\n바디라고합니다 사실 get에는 없지만 일단 확인 해보는겁니다...\n";


	cookie = "GET /test HTTP/1.1\r\nHost: foo.example\r\nCookie: PHPSESSID=298zf09hf012fh2; sid=34; csrftoken=u32t4o3tb3gg43; _gat=1\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: 27\r\n\r\nfield1=value1&field2=value2";
	basic_post = "GET /test HTTP/1.1\r\nHost: foo.example\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: 27\r\n\r\nfield1=value1&field2=value2";
	basic_get = "GET /test HTTP/1.1\r\nHost: foo.example\r\nContent-Type: application/x-www-form-urlencoded\r\n\r\n";
	chunked = "GET /test HTTP/1.1\r\n"
			"Content-Type: text/plain\r\n"
			"Transfer-Encoding: chunked\r\n\r\n";
			c1 = "7\r\nMozilla\r\n";
			c2 = "11\r\nDeveloper Network\r\n";
			c3 = "0\r\n\r\n";
			// 어디선 "0\r\n\r\n" 고 어디는 "0\r\n"라 나중에 수정해야됨

	sep1 = "GET /test HTTP/1.1\r\nHost: fo";
	sep2 ="o.example\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Lengt";
	sep3 = "h: 27\r\n\r\nfield1=value1&field2=value2";

/* ===== =============== execute*/
	std::cout << "sep" << std::endl;
	HttpreqHandler req1;
	req1.handle(&sep1);
	req1.handle(&sep2);
	req1.handle(&sep3);


	std::cout << "\n\nbasic_post" << std::endl;
	HttpreqHandler req2;
	req2.handle(&basic_post);

	std::cout << "\n\nbasic_get" << std::endl;
	HttpreqHandler req4;
	req4.handle(&basic_get);

	std::cout << "\n\nchunked" << std::endl;
	HttpreqHandler req3;
	req3.handle(&chunked);
	req3.handle(&c1);
	req3.handle(&c2);
	req3.handle(&c3);

	std::cout << "\n\ncookie" << std::endl;
	HttpreqHandler req5;
	req5.handle(&cookie);

	return (0);
}
