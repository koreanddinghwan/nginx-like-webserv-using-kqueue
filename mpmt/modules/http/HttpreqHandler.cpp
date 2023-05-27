#include "HttpreqHandler.hpp"

void *HttpreqHandler::handle(void *e) 
{
	std::string *req;
	
	req = static_cast<std::string *>(e);
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
		else if (_pended && _messageState == seperate)
			parseSeperate(*req);
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
		_messageState = seperate;
	else
	{
		pos = _buf.find("Transfer-Encoding: chunked");
		if (pos != std::string::npos) // chunked는 septerate하게 안들어온다고 가정, 나중에 수정해야될수도
		{	
			_messageState = chunked;
			_chunkedWithoutBodyBuf.append(_buf);
		}
		else
		{
			if (checkSeperate(bodyPos))
				_messageState = seperate;
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

void HttpreqHandler::parseContentLength(void)
{
	int prevPos, pos;
	char *endptr = NULL;
	std::string lengthStr;

	prevPos = _buf.find("Content-Length: ");
	if (prevPos == std::string::npos)
		return ;
	pos = _buf.find(CRLF, prevPos);
	if (pos == std::string::npos)
		return ;
	//15 == Content-Length길이
	lengthStr = _buf.substr(prevPos + 15, pos - prevPos - 15);
	_contentLength = std::strtod(lengthStr.c_str(), &endptr);
}

bool HttpreqHandler::checkSeperate(int pos)
{
	std::string line;

	parseMethod("");
	//post가 아니면 contentlength가 없으니까 basic
	if (_method.compare("POST"))
		return false;
	parseContentLength();
	line = _buf.substr(pos + 4);
	if (line.length() != _contentLength)
		return true;
	return false;
}

/* =============== chunked parse =============== */

int HttpreqHandler::parseChunkedLength(std::string req, int *pos)
{
	std::string line;

	if (req.find(CRLF2) != std::string::npos)
		return (0);
	*pos = req.find(CRLF);
	line = req.substr(0, *pos);
	return (convertHexToDec(line));
}

std::string HttpreqHandler::parseChunkedBody(std::string req, int *pos)
{
	std::string line;
	int	endPos;

	endPos = req.find(CRLF, *pos + 2);
	line = req.substr(*pos + 2, endPos - *pos - 2);
	return (line);
}

void HttpreqHandler::parseChunked(std::string req)
{
	int pos, len = 0;
	std::string line, body;
	
	len = parseChunkedLength(req, &pos);
	if (len == 0)
	{
		_pended = false;
		_buf.clear();
		_buf.append(_chunkedWithoutBodyBuf);
		_buf.append(_bodyBuf);
		return ;
	}
	line = parseChunkedBody(req, &pos);
	if (len != line.length())
		std::cout << "parseChunked 예외처리 해야됨..." << std::endl;
	_bodyBuf.append(line);
	_contentLength += len;
	appendBuf(line);
}

/* ============================================= */

/* ============== seperate parse =============== */

void HttpreqHandler::findMethod(void)
{
	int pos;
	std::string line;

	if ((pos = _buf.find(CRLF)) != std::string::npos)
	{
		line = _buf.substr(0, pos);
		parseMethod(line);
	}
}

void HttpreqHandler::appendBodyBuf(std::string req)
{
	int pos;
	std::string line;

	if ((pos = _buf.find(CRLF2)) == std::string::npos)
		return ;
	if (!_bodyBuf.length())
	{
		line = _buf.substr(pos + 4);
		_bodyBuf.append(line);
	}
	else
		_bodyBuf.append(req);
}

void HttpreqHandler::parseSeperate(std::string req)
{
	int pos;
	std::string line;

	if (_method.empty())
		findMethod();
	if (!_method.compare("POST")) // POST면 length찾고 body랑 length값 확인해야됨
	{ 
		if (!_contentLength)
		{
			parseContentLength();
			if ((pos = _buf.find(CRLF2)) != std::string::npos)
			{
				line = _buf.substr(pos);
				appendBodyBuf(line);
			}
		}
		else
			appendBodyBuf(req);
		if (_contentLength && _bodyBuf.length() == _contentLength)
			_pended = false;
	}
	else // POST 아니면, body없음, cflf2가 마지막인거임
	{
		if (_buf.find(CRLF2) != std::string::npos)
			_pended = false;
	}
}
/* ============================================= */


/* =================== cookie =================== */
void HttpreqHandler::saveSid(std::string key, std::string value)
{
	// if (!key.compare("sid"))
	if (key == "sid")
	{
		_sid = value;
		return ;
	}	
	return ;
}

void HttpreqHandler::insertCookieMap(std::string line, int *prevPos, int *pos)
{
	std::string key, value;

	*prevPos += line.length() + 2;
	*pos = line.find("=");
	key = line.substr(0, *pos);
	value = line.substr(*pos + 1);
	_info.reqCookieMap.insert(std::make_pair(key,value));
	saveSid(key, value);

}

void HttpreqHandler::parseCookie(void)
{
	int pos, prevPos = 0;
	std::string cookies;
	std::string line;
	std::map<std::string, std::string>::iterator it;

	if ((it = _info.reqHeaderMap.find("Cookie")) == _info.reqHeaderMap.end())
		return ;
	cookies = it->second;
	while ((pos = cookies.find("; ", prevPos)) != std::string::npos)
	{
		line = cookies.substr(prevPos, pos - prevPos);
		insertCookieMap(line, &prevPos, &pos);
	}
	line = cookies.substr(prevPos);
	insertCookieMap(line, &prevPos, &pos);
}
/* ============================================= */


/* =================== parse =================== */
void HttpreqHandler::parseStartLine(std::string line) 
{
	int pos, prevPos = 0;
	std::string subLine;

	while ((pos = line.find(" ", prevPos)) != std::string::npos)
	{
		subLine = line.substr(prevPos, pos - prevPos);
		if (_info.method.empty())
			_info.method = subLine;
		else if (_info.path.empty())
		{
			_info.path = subLine;
			_info.httpVersion = line.substr(pos + 1);
		}
		prevPos += subLine.length() + 1;
	}
}

bool HttpreqHandler::parseHeader(std::string line){
	int pos;
	std::string key, value;

	pos = line.find(": ");
	if (pos == std::string::npos)
		return false;// 종료, header 끝나고 body로 넘어가야됨
	key = line.substr(0, pos);
	value = line.substr(pos + 2);
	_info.reqHeaderMap.insert(std::make_pair(key,value));
	return true;
}

void HttpreqHandler::parseBody(void)
{
	int pos;
	std::string line;

	if (_info.method.compare("POST")) 
		return ;
	pos = _buf.find(CRLF2);
	line = _buf.substr(pos + 4);
	/* 
		get, delete는 body가 없긴 한데 테스터가 어떻게 넣어줄지 몰라서
		조건 method 확인하는 거 + line.empty() 수정하기 +
		congig max_client_length && header contentLength 확인하고 수정..할거 많네 
	*/
	//if (_info.method.compare("POST") && !line.empty())
		_info.body = line;
}

void HttpreqHandler::parse(void)
{
	int pos, prevPos = 0;
	std::string line;

	while((pos = _buf.find(CRLF, prevPos)) != std::string::npos)
	{
		line = _buf.substr(prevPos, pos - prevPos);
		prevPos += line.length() + 2;
		//startLine
		if (_info.method.empty())
			parseStartLine(line);
		//header
		else
		{
			if (!parseHeader(line))
				break ;
		}
	}
	//body
	parseCookie();
	parseBody();
}
/* ============================================= */


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
	if (getHasSid())
		std::cout << _sid << "??"<< std::endl;
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
