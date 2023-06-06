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

std::string HttpreqHandler::parseChunkedBody(int lenPos, int bodyPos)
{
	std::string line;

	// except 던지면서 임시변수 생성한 거 바로 던지게 하는 거 생각해보기....
	line = _bodyBuf.substr(lenPos + 2, bodyPos - lenPos - 2);
	return line;
}

void HttpreqHandler::splitChunked(int lenPos, int bodyPos)
{
	int len = 0;
	std::string line;
  
	len = parseChunkedLength(lenPos);
	line = parseChunkedBody(lenPos, bodyPos);
	if (len < line.length())
	{
		_event->setStatusCode(413);
		throw std::exception();
	}
	_contentLength += len;
	_info.body.append(line);
}

void HttpreqHandler::parseChunked(std::string req) 
{
	int pos  = 0;
	std::string line;
	std::stringstream ss;

	if (_bodyPended) // 청크 헤더 다 들어오고 바디 덜들어옴.
	{
		if (_bodyBuf.empty())
			_bodyBuf.append(req);
		while (!_bodyBuf.empty() && _bodyBuf != "0\r\n\r\n")
		{
			std::cout << "in" <<std::endl;
			pos = _bodyBuf.find(CRLF);
			if (!pos && !_hasContentLength) // 길이 안들어옴
			{

			std::cout << "1" <<std::endl;
				return ;
			}
			else if (!pos && _hasContentLength) // body 일부만 들어옴
			{
			std::cout << "2" <<std::endl;
				_info.body.append(_bodyBuf);
				_bodyBuf.clear();
				return ;
			}
			else if (pos && !_hasContentLength) // 길이 들어옴
			{
			std::cout << "3" <<std::endl;
				_hasContentLength = true;
				_chunkedLength = parseChunkedLength(pos);
				_contentLength += _chunkedLength;
				_infoBodyIdx = _info.body.length();
				_bodyBuf.erase(0, pos + 2);
			}
			else if (pos && _hasContentLength) // body 다 들어옴
			{
			std::cout << "4" <<std::endl;
				_hasContentLength = false;
				line = parseChunkedBody(0, pos);
				_info.body.append(line);
				if (_info.body.length() - _infoBodyIdx > _chunkedLength)
				{
					_event->setStatusCode(413);
					throw std::exception();
				}
				_bodyBuf.erase(0, pos + 2);
			}
		}
		if (_bodyBuf == "0\r\n\r\n")
		{
			std::cout << "find 0 end" << std::endl;
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
	parseCookie();
}
/* ============================================= */
