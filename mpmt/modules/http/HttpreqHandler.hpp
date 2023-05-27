#ifndef HTTPREQHANDLER_HPP
# define HTTPREQHANDLER_HPP

#include <iostream>
#include "./HttpRequestInfo.hpp"
#include "../../interface/IHandler.hpp"

#define CRLF "\r\n"
#define CRLF2 "\r\n\r\n"

enum messageStateEnum {
	chunked = 1,
	seperate,
	basic
};

class HttpreqHandler : public IHandler
{
private:
	//httpRequestInfo *info;
	/* ======== 패킷 모두 전송 전까지 사용할 변수 ======= */
	//패킷 전송 방식(chunked, seperate, basic)
	messageStateEnum _messageState;

	std::string	_buf;
	std::string	_method;
	std::string _bodyBuf;
	std::string _chunkedWithoutBodyBuf;

	int _contentLength;
	bool _pended;
	/* ========================================== */

	/* ======== 패킷 모두 전송 후에 사용할 변수 ======= */
	//파싱 후 result
	struct httpRequestInfo _info;
	//std::map<std::string, std::string> reqHeaderMap;
	/* ========================================== */

public:
	HttpreqHandler();
	~HttpreqHandler();

public:
	void *handle(void *data);

	//utils
	void printReq(void);
	bool getIsPending(void) const;
	const httpRequestInfo &getRequestInfo(void) const;

private:
	//init
	void initRequest(std::string req);
	void initVar(void);
	void initMessageState(void);
	void initPendingState(void);

	//init utils
	void parseContentLength(void);
	void parseMethod(std::string line);
	bool checkSeperate(int pos);

	//parse
	void parse(void);
	void parseStartLine(std::string line);
	bool parseHeader(std::string line);
	void parseBody(void);

	//chunked
	void parseChunked(std::string req);
	int parseChunkedLength(std::string req, int *pos);
	std::string parseChunkedBody(std::string req, int *pos);

	//seperate
	void parseSeperate(std::string req);
	void findMethod(void);
	void appendBodyBuf(std::string req);
	
	void appendBuf(std::string buf);
	std::string getBuf(void) const;
};

int convertHexToDec(std::string line);

#endif
