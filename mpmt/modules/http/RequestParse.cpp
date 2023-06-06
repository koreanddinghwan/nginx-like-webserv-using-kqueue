#include "HttpreqHandler.hpp"
#include <algorithm>
#include <ostream>


/* =================== undefine =================== */
void HttpreqHandler::parseUndefined(void)
{
	int pos;

	pos = _buf.find(CRLF2);
	if (pos == std::string::npos)
		return ;
	_headerPended = false;
	initMessageState();
	initPendingState();
}

/* ============================================== */

/* =============== chunked parse =============== */

int HttpreqHandler::parseChunkedLength(int *startPos)
{
	std::string line;
	int pos;

	pos = _bodyBuf.find(CRLF, *startPos);
	line = _bodyBuf.substr(*startPos, pos - *startPos);
	*startPos = pos + 2;
	return (convertHexToDec(line));
}

std::string HttpreqHandler::parseChunkedBody(int *startPos)
{
	std::string line;
	int	pos;

	pos = _bodyBuf.find(CRLF, *startPos);
	line = _bodyBuf.substr(*startPos, pos - *startPos);
	*startPos = pos + 2;
	return (line);
}

void HttpreqHandler::splitChunked(void)
{
	int pos = 0, len = 0;
	int endPos = _bodyBuf.length() - 5;
	std::string line, buf;
    std::stringstream ss;
	
	while (pos != endPos)
	{
		len = parseChunkedLength(&pos);
		line = parseChunkedBody(&pos);
		if (len < line.length())
		{
			_event->setStatusCode(413);
			throw std::exception();
		}
		_contentLength += len;
		buf.append(line);
	}
    ss << _contentLength;
    _info.contentLength = ss.str();
	_bodyBuf.clear();
	_bodyBuf = buf;
}

void HttpreqHandler::parseChunked(std::string req) 
{
	int pos = 0, endPos = 0;
	std::string line;
	
	if (_headerPended) //청크 헤더 덜들어옴
	{
		pos = _buf.find(CRLF2);
		if (pos == std::string::npos)
			return ;
		_headerPended = false;
		_bodyPended = true;
		_chunkedWithoutBodyBuf.append(_buf.substr(0, pos + 4));
	}
	if (_bodyPended) // 청크 헤더 다 들어오고 바디 덜들어옴. 바디 끝 들어올 때 까지 파싱 x
	{
		endPos = _buf.find("0\r\n\r\n");
		if (endPos == std::string::npos)
			return ;
		// 바디 다 들어옴, 길이, 바디 분리
		pos = _buf.find(CRLF2);
		_bodyBuf = _buf.substr(pos + 4);
		_pended = false;
		_bodyPended = false;
		splitChunked();
		_buf.clear();
		_buf.append(_chunkedWithoutBodyBuf);
		_buf.append(_bodyBuf);		
	}
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
			if (_headerPended)
				_headerPended = false;
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
	char *endptr;

	if (key == "sid")
	{
		_sidString = value;
		_sid = std::strtod(value.c_str(), &endptr);
	}
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
	std::cout<<"key : "<<key<<" value : "<<value<<std::endl;
	_info.reqHeaderMap.insert(std::make_pair(key,value));
	saveGenericHeader(key, value);
	return true;
}

void HttpreqHandler::parseBody(void)
{
	int pos;

	pos = _buf.find(CRLF2);
	_info.body.assign(_buf.begin() + pos + 4, _buf.end());
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
