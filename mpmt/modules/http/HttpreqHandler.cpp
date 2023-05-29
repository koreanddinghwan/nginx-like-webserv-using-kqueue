#include "HttpreqHandler.hpp"
#include <stdexcept>

void *HttpreqHandler::handle(void *data) 
{
	_event = static_cast<Event *>(data);
	std::string *req = _event->getBuffer();
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
	return _event;
}

void HttpreqHandler::initMessageState(void)
{
	int bodyPos, pos;

	bodyPos = _buf.find(CRLF2);
	if (bodyPos == std::string::npos)
		_messageState = separate;
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
	_info.contentLength = lengthStr;
	_contentLength = std::strtod(lengthStr.c_str(), &endptr);
	return (true); // 헤더랑 밸류 모두 있음, 메소드 모름
}

bool HttpreqHandler::checkSeparate(int CRLF2Pos)
{
	std::string line;
	bool hasContentLen;
	int pos;

	parseMethod("");
	hasContentLen = parseContentLength();
	pos = _buf.find("", CRLF2Pos);
	line = _buf.substr(CRLF2Pos + 4, pos - CRLF2Pos);
	if (!hasContentLen)
	{
		if (!line.empty())
		{
			_event->setStatusCode(411);
			throw std::exception();
		}
	}
	else if (hasContentLen)
	{
		if (line.length() > _contentLength)
		{
			_event->setStatusCode(413);
			throw std::exception();//std Exception code안맞춰서 날려라
		}
		if (line.length() < _contentLength)
		{
			appendBodyBuf(line);
			return true;
		}
	}
	return false;
}

void HttpreqHandler::checkMethod(void)
{
	if (_info.method == "GET" || _info.method == "POST" || _info.method == "DELETE" ||
		_info.method == "PUT" || _info.method == "PATCH" || _info.method == "HEAD")
		return ;
	std::cout << _info.method << " " << _info.method.length() << std::endl;
	_event->setStatusCode(405);
	throw std::exception();

}

void HttpreqHandler::checkHttpVersion(void)
{
	if (_info.httpVersion != "HTTP/1.1")
	{
		_event->setStatusCode(400);
		throw std::exception();
	}
}

void HttpreqHandler::parseQueryParam(std::string line, int *prevPos, int *pos)
{
	std::string key, value;

	*prevPos += line.length() + 1;
	*pos = line.find("=");
	key = line.substr(0, *pos);
	value = line.substr(*pos + 1);
	_info.queryParamsV.push_back(std::make_pair(key, value));
}

void HttpreqHandler::checkQueryParam(void)
{
	int pos, prevPos, questionPos = 0;
	std::string line;

	if ((questionPos = _info.path.find("?")) != std::string::npos)
	{
		_info.queryParam = _info.path.substr(questionPos + 1);
		while((pos = _info.queryParam.find("&", prevPos)) != std::string::npos)
		{
			line = _info.queryParam.substr(prevPos, pos - prevPos);
			parseQueryParam(line, &prevPos, &pos);
		}
		// 쿼리파람이 uri의 마지막이라고 생각하면
		line = _info.queryParam.substr(prevPos);
		parseQueryParam(line, &prevPos, &pos);
		_info.path.erase(questionPos, _info.queryParam.length() + 1);
	}
}

void HttpreqHandler::checkStartLine(void)
{
	checkMethod();
	checkHttpVersion();
	checkQueryParam();
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

const httpRequestInfo &HttpreqHandler::getRequestInfo(void) const { return _info; }
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

//https://stackoverflow.com/questions/154536/encode-decode-urls-in-c
std::string encodePercentEncoding(const std::string& str)
{
  std::ostringstream escaped;
  escaped.fill('0');
  escaped << std::hex;

  for (std::string::const_iterator i = str.begin(), n = str.end(); i != n; ++i)
  {
    std::string::value_type c = (*i);
    // Keep alphanumeric and other accepted characters intact
    if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~')
    {
      escaped << c;
      continue;
    }
    // Any other characters are percent-encoded
    escaped << std::uppercase;
    escaped << '%' << std::setw(2) << static_cast<int>(static_cast<unsigned char>(c));
    escaped << std::nouppercase;
  }
  return (escaped.str());
}

std::string urlDecode(std::string &SRC)
{
    std::string ret;
    char ch;
    int i, ii;
    for (i=0; i<SRC.length(); i++) {
        if (SRC[i]=='%') {
            sscanf(SRC.substr(i+1,2).c_str(), "%x", &ii);
            ch=static_cast<char>(ii);
            ret+=ch;
            i=i+2;
        } else {
            ret+=SRC[i];
        }
    }
    return (ret);
}