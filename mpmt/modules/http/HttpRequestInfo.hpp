#ifndef HTTPREQUESTINFO_HPP
# define HTTPREQUESTINFO_HPP

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


enum method {
	GET = 1,
	POST,
	DELETE
};

#define HTTPV10 "HTTP/1.0" // HTTP/1.0
#define HTTPV11 "HTTP/1.1" // HTTP/1.1
#define HTTPV20 "HTTP/2.0" // HTTP/2.0

enum RequestBodyType
{
	SingleResource = 1,
	
};

//필수적인 것만 파싱하고, 너미지는 char *others에 넣는게 나을듯요
struct httpRequestInfo
{
	/*Start line*/
	method Method; //get,post,delete
	char *Path; //request target, url임 대부분
	char *HttpVersion; //http 버전


	/*Headers*/
		/*Requeset headers*/
		char *Host;
		char *UserAgent;
		char *Accept;
		char *AcceptLanguage;
		char *AcceptEncoding;

		/*General headers*/
		int Connection;
		int UpgradeInsecureRequesets;

		/*Representation Headers*/
		char *ContentType;
		int ContentLength;
	
	/*body*/


	/*others*/
	char *others;
};

#endif
