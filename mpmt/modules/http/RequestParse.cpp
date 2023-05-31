#include "HttpreqHandler.hpp"


/* =============== chunked parse =============== */

int HttpreqHandler::parseChunkedLength(std::string req, int *pos)
{
	int endPos;
	std::string line;

	//chunked end
	if ((endPos = req.find(CRLF2)) != std::string::npos)
	{
		if (req.length() == 5 && req[endPos - 1] == '0')
			return (0);
		_event->setStatusCode(411);
		throw std::exception();
		// 설마 CRLF2 뒤에 문자 더 있겠나
	}
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

// 청크드 고쳐야 하는게 청크드도 파셜로 들어옴
// 헤더만 저장 하고 0crlf crlf들어올 때 까지 계속 리드하고 한 번에 파싱하는게 맞는 거 같습니다.

void HttpreqHandler::parseChunked(std::string req) 
{
	int pos = 0, len = 0;
	std::string line;
	
	if (_headerPended)
	{
		pos = _buf.find(CRLF2);
		if (pos == std::string::npos)
			return ;
			// throw std::exception();
		_headerPended = false;
		_chunkedWithoutBodyBuf.append(_buf.substr(0, pos));
	}
	len = parseChunkedLength(req, &pos);
	if (len == 0)
	{
		_pended = false;
		_bodyPended = false;
		_buf.clear();
		_buf.append(_chunkedWithoutBodyBuf);
		_buf.append(_bodyBuf);
		return ;
	}
	line = parseChunkedBody(req, &pos);
	if (len < line.length())
	{
		_event->setStatusCode(413);
		throw std::exception();
	}
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
	if (_headerPended)
		_headerPended = false;
	if (!_bodyBuf.length())
	{
		line = _buf.substr(pos + 4);
		_bodyBuf.append(line);
	}
	else
		_bodyBuf.append(req);
}

void HttpreqHandler::parseSeparate(std::string req)
{
	int pos;
	std::string line;

	if (_method.empty())
		findMethod();
	if (_bodyBuf.empty())
	{
		if ((pos = _buf.find(CRLF2)) != std::string::npos)
		{
			if (!checkSeparate(pos))
			{
				_pended = false;
				_bodyPended = false;
			}
		}
	}
	else
	{
		appendBodyBuf(req);
		if (_contentLength == _bodyBuf.length())
		{
			_pended = false;
			_bodyPended = false;
		}
		else if (_contentLength < _bodyBuf.length())
		{
			_event->setStatusCode(413);
			throw std::exception();
		}
	}
}
/* ============================================= */


/* =================== cookie =================== */
void HttpreqHandler::saveSid(std::string key, std::string value)
{
	if (key == "sid")
		_sid = value;
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
	int pos = 0, prevPos = 0;
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
	checkStartLine();
}

void HttpreqHandler::saveGenericHeader(std::string key, std::string value)
{
	if (key == "Host")
		_info.host = value;
	if (key == "Content-Length")
		_info.contentLength = value;
	if (key == "Content-Type")
		_info.contentType = value;
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
	saveGenericHeader(key, value);
	return true;
}

void HttpreqHandler::parseBody(void)
{
	int pos;
	std::string line;

	pos = _buf.find(CRLF2);
	line = _buf.substr(pos + 4);
	_info.body = line;
}

void HttpreqHandler::parse(void)
{
	int pos = 0, prevPos = 0;
	std::string line;

	while((pos = _buf.find(CRLF, prevPos)) != std::string::npos)
	{
		line = _buf.substr(prevPos, pos - prevPos);
		prevPos += line.length() + 2;
		//startLine
		if (_info.method.empty())
			parseStartLine(urlDecode(line));
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