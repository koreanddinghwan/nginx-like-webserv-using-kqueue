#include "HttpreqHandler.hpp"
#include <algorithm>
#include <ostream>
#include <sstream>
#include <string>


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

int HttpreqHandler::parseChunkedLength(int pos)
{
	std::string line;

	line = _bodyBuf.substr(0, pos);
	return (convertHexToDec(line));
}

std::string HttpreqHandler::parseChunkedBody(int pos)
{
	std::string line;

	// except 던지면서 임시변수 생성한 거 바로 던지게 하는 거 생각해보기....
	line = _bodyBuf.substr(0, pos);
	return line;
}

void HttpreqHandler::parseChunked(std::string req) 
{
	int pos  = 0;
	std::string line;
	std::stringstream ss;

	if (!_headerPended && _bodyPended) // 청크 헤더 다 들어오고 바디 덜들어옴.
	{
		if (!_flag)
			_bodyBuf.append(req);
		else
			_flag = false;
		while (!_bodyBuf.empty() && _bodyBuf != "0\r\n\r\n")
		{
			pos = _bodyBuf.find(CRLF);
			if (pos == std::string::npos && !_hasContentLength) // 길이 안들어옴
			{
				//_currentBodyLength = 0;
				return ;
			}
			else if (pos == std::string::npos && _hasContentLength) // body 일부만 들어옴
			{
				if (_bodyBuf[_bodyBuf.length() - 1] == '\r')
					return ;
				_info.body.append(_bodyBuf);
				_currentBodyLength += _bodyBuf.length();
				_bodyBuf.clear();
				return ;
			}
			else if (pos != std::string::npos && !_hasContentLength) // 길이 들어옴
			{
				_chunkedLength = parseChunkedLength(pos);
				if (!_chunkedLength)
					return ;
				_hasContentLength = true;
				_infoBodyIdx = _info.body.length();
				_contentLength += _currentBodyLength;
				_currentBodyLength = 0;
				_bodyBuf.erase(0, pos + 2);
			}
			else if (pos != std::string::npos && _hasContentLength) // body 다 들어옴
			{
				_hasContentLength = false;
				line = parseChunkedBody(pos);
				_info.body.append(line);
				if (_info.body.length() - _infoBodyIdx > _chunkedLength)
				{
					std::cout << "길이 안맞음" << std::endl;
					std::cout << _info.body.length() << std::endl;
					std::cout << _infoBodyIdx << std::endl;
					std::cout << _chunkedLength << std::endl;
					_event->setStatusCode(413);
					throw std::exception();
				}
				_bodyBuf.erase(0, pos + 2);
			}
		}
		if (_bodyBuf == "0\r\n\r\n")
		{
			_bodyPended = false;
			_pended = false;
			ss << _contentLength;
			_info.contentLength = ss.str();
		}
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

void HttpreqHandler::parseWithoutBody(void)
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
	parseCookie();
}
/* ============================================= */
