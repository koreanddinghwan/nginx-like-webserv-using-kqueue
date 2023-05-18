#ifndef HTTPREQUEST_HPP
# define HTTPREQUEST_HPP

# include <iostream>
# include "../../interface/IHttpBlock.hpp"
# include "HttpRequestInfo.hpp"
# include <string.h>

#define CRLF "\r\n"

enum {
	chunked = 1,
	seperate,
	others
};



class HttpRequest
{
	public:
		HttpRequest(void);
		HttpRequest(std::string req);
		HttpRequest(const HttpRequest& obj);
		~HttpRequest(void);
		HttpRequest& operator=(const HttpRequest& obj);

	public: 
		void setBuf(std::string buf);
		std::string getBuf(void) const;
		void initRequest(void);
		void parseOthers(void);
		void parseStartLine(std::string line);
		void parseHeader(std::string line);
		void parseBody(void);

		void printReq(void);

	private:
		//파싱 후 예쁘게 담을 구조체
		struct httpRequestInfo _reqInfo;

		//enum으로 header 어떻게 들어오는지
		int state;

		//파싱하는데 사용할 버퍼
		//body까지 다 받은 후 아래 변수에 쪼개기
		std::string _buf;
		//간략하게 3필드로 나눌 변수 사용 x
		std::string _startLine;
		std::string _header;
		std::string _body;

		//request Header만 담을 map
		// map은 순서 보장 안하니까 상의하고 다른 자료구조로 바꾸기
		std::map<std::string, std::string> _reqHeaderMap;

};

#endif

