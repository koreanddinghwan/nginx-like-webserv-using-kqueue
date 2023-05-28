#include "HttpreqHandler.hpp"

void *HttpreqHandler::handle(void *data) 
{
	Event *e = static_cast<Event *>(data);
	std::string *req = e->getBuffer();

	/*
	 처음 들어온 req massage
	*/
	if (_buf.empty())
		initRequest(*req);
	else
	{
		appendBuf(*req);
		if (_pended && _messageState == chunked)
			parseChunked(*req);
		else if (_pended && _messageState == separate)
			parseSeparate(*req);
	}
	/*
	fulfilled state message
	*/
	if (!_pended)
	{
		parse();
		printReq();
	}	
	return e;
}

void HttpreqHandler::initMessageState(void)
{
	int bodyPos, pos;

	bodyPos = _buf.find(CRLF2);
	if (bodyPos == std::string::npos)
		_messageState = separate;
	else
	{
		_findCRLF2 = true;
		pos = _buf.find("Transfer-Encoding: chunked");
		if (pos != std::string::npos) // chunked는 septerate하게 안들어온다고 가정, 나중에 수정해야될수도
		{	
			_messageState = chunked;
			_chunkedWithoutBodyBuf.append(_buf);
		}
		else
		{
			if (checkSeparate(bodyPos))
				_messageState = separate;
			else
				_messageState = basic;
		}
	}
}

void HttpreqHandler::initPendingState(void)
{
	if (_messageState != basic)
		_pended = true;
}

void HttpreqHandler::initVar(void)
{
	_contentLength = 0;
	_buf.clear();
	_method.clear();
	_bodyBuf.clear();
	_chunkedWithoutBodyBuf.clear();
	_hasContentLength = false;
	_findCRLF2 = false;
}

void HttpreqHandler::initRequest(std::string req)
{
	initVar();
	appendBuf(req);
	// chunked, basic,seperate 메세지 형식 구분하는 함수
	initMessageState();
	initPendingState();
}

void HttpreqHandler::parseMethod(std::string line)
{
	int pos;

	if (line.compare(""))
		pos = _buf.find(" ");
	else
	 	pos = line.find(" ");
	if (pos != std::string::npos)
		_method = _buf.substr(0, pos);
}

/*
	init의 경우 checkSep에서 넘어왔을 때
	CRLF2찾은 경우만 들어옴
*/
bool HttpreqHandler::parseContentLength(void) // findContentLength
{
	int prevPos, pos;
	char *endptr = NULL;
	std::string lengthStr;

	prevPos = _buf.find("Content-Length: ");
	if (prevPos == std::string::npos)
		return (false);
	pos = _buf.find(CRLF, prevPos);
	//15 == Content-Length길이
	lengthStr = _buf.substr(prevPos + 15, pos - prevPos - 15);
	_contentLength = std::strtod(lengthStr.c_str(), &endptr);
	return (true); // 헤더랑 밸류 모두 있음, 메소드 모름
}

bool HttpreqHandler::checkSeparate(int CRLF2Pos)
{
	std::string line;
	bool hasContentLen;

	parseMethod("");
	hasContentLen = parseContentLength();
	line = _buf.substr(CRLF2Pos + 4);
	if (!hasContentLen)
	{
		if (!line.empty())
			throw HttpException(411);
	}
	else if (hasContentLen)
	{
		if (line.length() > _contentLength)
			throw HttpException(413);
		if (line.length() < _contentLength)
		{
			appendBodyBuf(line);
			return true;
		}
	}
	return false;
}

/* =============== constructor ================== */
HttpreqHandler::HttpreqHandler()
	: _buf(""), _messageState(basic), _pended(false)
{
	//info = new HttpreqHandlerInfo();
}

HttpreqHandler::~HttpreqHandler()
{
	//delete this->info;
}
/* ============================================= */


/* =============== utils ================== */
std::string HttpreqHandler::getBuf() const { return _buf; }

void HttpreqHandler::appendBuf(std::string req) { _buf.append(req); }

bool HttpreqHandler::getIsPending(void) const { return _pended; }

bool HttpreqHandler::getHasSid(void) const { return (!_sid.empty() ? true : false); }

std::string HttpreqHandler::getSid(void) const { return _sid;}

const httpRequestInfo &HttpreqHandler::getRequestInfo(void) const
{ return _info; }
/* ============================================= */

void HttpreqHandler::printReq(void)
{
	std::cout<<"\033[35m"<<"=============Request Result============"<<std::endl;
	std::cout<<"\033[35m"<<"Start Line"<<std::endl<<std::endl;
	std::cout<<"\033[35m"<<_info.method<<std::endl;
	std::cout<<"\033[35m"<<_info.path<<std::endl;
	std::cout<<"\033[35m"<<_info.httpVersion<<std::endl<<std::endl;
	std::cout<<"Header"<<std::endl<<std::endl;
	for (std::map<std::string, std::string>::iterator it = _info.reqHeaderMap.begin(); it != _info.reqHeaderMap.end(); it++)
		std::cout<<"\033[35m"<<"key:"<< it->first <<" value:"<<it->second<<std::endl;
	std::cout<<"\nBody"<<std::endl<<std::endl;
	std::cout<<"\033[35m"<<_info.body<<std::endl<<std::endl;
	std::cout<<"Cookie"<<std::endl<<std::endl;
	for (std::map<std::string, std::string>::iterator it = _info.reqCookieMap.begin(); it != _info.reqCookieMap.end(); it++)
		std::cout<<"\033[35m"<<"key:"<< it->first <<" value:"<<it->second<<std::endl;
	std::cout<<"\033[35m"<<"=============Request Result End============"<<std::endl;
}

int convertHexToDec(std::string line)
{
	int hex, dec = 0;
	const char *c;
	char *endptr;
	std::string tmp;

	for (std::string::iterator it = line.begin(); it != line.end(); it++)
	{
		tmp = *it;
		c = tmp.c_str();

		switch(*c) {
			case 'A' : c = "10"; break;
			case 'B' : c = "11"; break;
			case 'C' : c = "12"; break;
			case 'D' : c = "13"; break;
			case 'E' : c = "14"; break;
			case 'F' : c = "15"; break;
			default: break;
		}
		hex = std::strtod(c, &endptr);
		dec *= 16;
		dec += hex;
	}
	return (dec);
}
