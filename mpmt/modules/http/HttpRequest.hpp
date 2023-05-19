#ifndef HTTPREQUEST_HPP
# define HTTPREQUEST_HPP

# include <iostream>
# include "../../interface/IHttpBlock.hpp"
# include "HttpRequestInfo.hpp"
# include <string>

#define CRLF "\r\n"
#define CRLF2 "\r\n\r\n"

enum messageStateEnum {
	chunked = 1,
	seperate,
	basic
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
		void initRequest(std::string req);
		
		//init utils
		void parseContentLength(void);
		void parseMethod(void);
		bool checkSeperate(int poscrlf2Pos);
		void initMessegeState(void);

		//basic
		void parseBasic(void);
		void parseStartLine(std::string line);
		void parseHeader(std::string line);
		void parseBody(void);

		//utils
		void printReq(void);
		void appendBuf(std::string req);

	private:

		/* ======== 패킷 모두 전송 전까지 사용할 변수 ======= */
		//패킷 전송 방식(chunked, seperate, basic)
		messageStateEnum _messageState;

		std::string	_buf;
		std::string	_method;

		int _contentLength;
		/* ========================================== */


		/* ======== 패킷 모두 전송 후에 사용할 변수 ======= */
		//파싱 후 result
		struct httpRequestInfo _reqInfo;
		//std::map<std::string, std::string> reqHeaderMap;

		/* ========================================== */
};

#endif

