#ifndef HTTPREQHANDLER_HPP
# define HTTPREQHANDLER_HPP

#include <iostream>
#include <sstream>
#include "../eventLoop/Event.hpp"
#include "./HttpRequestInfo.hpp"
#include "../../interface/IHandler.hpp"
#include "../../exceptions/httpException.hpp"

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

	int _chunkedLength;
	int _infoBodyIdx;
	bool _hasContentLength;

	//구차..
	bool _flag;
	/* ========================================== */

	/* ======== 패킷 모두 전송 후에 사용할 변수 ======= */
	//파싱 후 result
	struct httpRequestInfo _info;

	Event *_event;

	bool _pended;
	bool _headerPended;
	bool _bodyPended;
	int 		_sid;
	std::string _sidString;
	/* ========================================== */

public:
	HttpreqHandler();
	~HttpreqHandler();

public:
	int _contentLength;
	int _currentBodyLength;
	void *handle(void *data);

	//getter
	bool getIsPending(void) const;
	bool isHeaderPending(void) const;
	bool isBodyPending(void) const;
	bool getHasSid(void) const;
	int getSid(void) const;
	std::string getSidString(void) const;
	const httpRequestInfo &getRequestInfo(void) const;

	void printReq(void);
private:
	//init
	void initRequest(std::string req);
	void initVar(void);
	void initMessageState(void);
	void initPendingState(void);

	//init utils
	bool parseContentLength(void);
	void parseMethod(std::string line);
	bool checkSeparate(int CRLF2Pos);

	//cookie
	void parseCookie(void);
	void insertCookieMap(std::string cookies, int *prevPos, int *pos);
	void saveSid(std::string key, std::string value);

	//parse
	void parseWithoutBody(void);
	void parseStartLine(std::string line);
	bool parseHeader(std::string line);
	void parseBody(void);
	void saveGenericHeader(std::string key, std::string value);
	void parseQueryParam(std::string line, int *prevPos, int *pos);

	//chunked
	void parseChunked(std::string req);
	void splitChunked(int pos, int endPos);
	int parseChunkedLength(int pos);
	std::string parseChunkedBody(int pos);

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

int convertHexToDec(std::string line);
std::string encodePercentEncoding(const std::string& str);
std::string urlDecode(std::string &SRC);

#endif
