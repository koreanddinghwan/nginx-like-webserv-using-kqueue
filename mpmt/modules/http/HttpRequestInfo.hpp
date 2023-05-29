#ifndef HTTPREQUESTINFO_HPP
# define HTTPREQUESTINFO_HPP

# include <iostream>
# include <map>
# include <string>

/*
 * https://developer.mozilla.org/en-US/docs/Web/HTTP/Messages
 * GET / HTTP/1.1
 * Host: localhost:8000
 * Connection: keep-alive
 * sec-ch-ua: "Not_A Brand";v="99", "Google Chrome";v="109", "Chromium";v="109"
 * sec-ch-ua-mobile: ?0
 * sec-ch-ua-platform: "macOS"
 * Upgrade-Insecure-Requests: 1
 * User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/109.0.0.0 Safari/537.36
 * Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,\*\/\*;q=0.8,application/signed-exchange;v=b3;q=0.9
 * Sec-Fetch-Site: none
 * Sec-Fetch-Mode: navigate
 * Sec-Fetch-User: ?1
 * Sec-Fetch-Dest: document
 * Accept-Encoding: gzip, deflate, br
 * Accept-Language: ko-KR,ko;q=0.9,en-US;q=0.8,en;q=0.7
 * */

#define HTTPV10 "HTTP/1.0" // HTTP/1.0
#define HTTPV11 "HTTP/1.1" // HTTP/1.1
#define HTTPV20 "HTTP/2.0" // HTTP/2.0

// enum RequestBodyType
// {
// 	SingleResource = 1,
	
// };

//필수적인 것만 파싱하고, 너미지는 char *others에 넣는게 나을듯요
struct httpRequestInfo
{
	enum method {
		GET = 1,
		POST,
		DELETE
	};
	/*Start line*/
	std::string method; //get,post,delete
	std::string path; //request target, url임 대부분
	std::string httpVersion; //http 버전


	/*Headers*/
	//request Header만 담을 map(map은 순서 보장 안하니까 상의하고 다른 자료구조로 바꾸기)
	std::map<std::string, std::string> reqHeaderMap;
	std::map<std::string, std::string> reqCookieMap;
		/*Requeset headers*/
		std::string host;
		//char *UserAgent;
		//char *Accept;
		//char *AcceptLanguage;
		//char *AcceptEncoding;

		///*General headers*/
		//int Connection;
		//int UpgradeInsecureRequesets;

		/*Representation Headers*/
		// char *ContentType;
		// int ContentLength;
	
	/*body*/
		std::string body;

	/*others*/
	// char *others;
};

#endif
