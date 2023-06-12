#ifndef HTTPREQHANDLER_HPP
# define HTTPREQHANDLER_HPP

#include <iostream>
#include <sstream>
#include <ostream>
#include <algorithm>
#include <stdexcept>
#include "./HttpRequestInfo.hpp"
#include "../eventLoop/Event.hpp"
#include "../http/HttpServer.hpp"
#include "../../interface/IHandler.hpp"

#define CRLF "\r\n"
#define CRLF2 "\r\n\r\n"

enum messageStateEnum {
	chunked = 1,
	separate,
	basic,
	undefined,
};

class HttpreqHandler : public IHandler
{
private:
	/* ======== 패킷 모두 전송 전까지 사용할 변수 ======= */
	//패킷 전송 방식(chunked, separate, basic)
	messageStateEnum _messageState;

	std::string	_buf;
	std::string	_method;
	std::string _bodyBuf;
	std::string _chunkedWithoutBodyBuf;

	size_t _contentLength;
	bool _hasContentLength;
	/* ========================================== */

	/* ======== 패킷 모두 전송 후에 사용할 변수 ======= */
	//파싱 후 result
	struct httpRequestInfo _info;

	Event *_event;

	bool _pended;
	bool _headerPended;
	bool _bodyPended;
	std::string _sid;
	/* ========================================== */

public:
	HttpreqHandler();
	~HttpreqHandler();

public:
	void *handle(void *data);

	//getter
	bool getIsPending(void) const;
	bool isHeaderPending(void) const;
	bool isBodyPending(void) const;
	bool getHasSid(void) const;
	std::string getSid(void) const;
	const httpRequestInfo &getRequestInfo(void) const;

private:
	//init
	void initRequest(std::string req);
	void initVar(void);
	void initMessageState(void);
	void initPendingState(void);

	//init utils
	bool parseContentLength(void);
	void parseMethod(std::string line);
	bool checkSeparate(size_t CRLF2Pos);

	//cookie
	void parseCookie(void);
	void insertCookieMap(std::string cookies, size_t *prevPos, size_t *pos);
	void saveSid(std::string key, std::string value);

	//parse
	void parse(void);
	void parseStartLine(std::string line);
	bool parseHeader(std::string line);
	void parseBody(void);
	void saveGenericHeader(std::string key, std::string value);
	void parseQueryParam(std::string line, size_t *prevPos, size_t *pos);

	//chunked
	void parseChunked(void);
	void splitChunked(void);
	size_t parseChunkedLength(size_t *startPos);
	std::string parseChunkedBody(size_t *startPos);

	//separate
	void parseSeparate(std::string req);
	void findMethod(void);
	void appendBodyBuf(std::string req);

	//undefine
	void parseUndefined(void);
	

	void appendBuf(std::string buf);
	std::string getBuf(void) const;

	//check start Line
	void checkStartLine(void);
	void checkMethod(void);
	void checkHttpVersion(void);
	void checkQueryParam(void);
};

size_t convertHexToDec(std::string line);
std::string encodePercentEncoding(const std::string& str);
std::string urlDecode(std::string &SRC);

#endif
