#include "HttpreqHandler.hpp"
#include "../http/HttpServer.hpp"
#include <stdexcept>

void *HttpreqHandler::handle(void *data) 
{
	_event = static_cast<Event *>(data);
	std::string req = HttpServer::getInstance().getHttpBuffer();
	/*
	 처음 들어온 req massage
	*/
	if (_buf.empty())
		initRequest(req);
	else
	{
		appendBuf(req);
		if (_pended && _messageState == undefined)
			parseUndefined();
		if (_pended && _messageState == chunked)
			parseChunked(req);
		else if (_pended && _messageState == separate)
			parseSeparate(req);
	}
	/*
	fulfilled header
	*/
	if (!_headerPended && _info.method.empty())
		parseWithoutBody();
	/*
	fulfilled state message
	*/
	if (!_pended && _messageState != chunked)
	{
		parseBody();
		/* printReq(); */
	}
	return _event;
}

void HttpreqHandler::initMessageState(void)
{
	int bodyPos, pos;

	bodyPos = _buf.find(CRLF2);
	if (bodyPos == std::string::npos)
	{
		_messageState = undefined;
		_headerPended = true;
	}
	else
	{
		pos = _buf.find("Transfer-Encoding: chunked"); // 청크 형식 맞춰 들어옴, 헤더는 다 들어옴
		if (pos != std::string::npos)
		{	
			_messageState = chunked;
			_bodyPended = true;
			_chunkedWithoutBodyBuf.append(_buf.substr(0, bodyPos + 4));
			_bodyBuf = _buf.substr(bodyPos + 4);
			_flag = true;
		}
		else
		{
			if (checkSeparate(bodyPos))
			{
				_messageState = separate;
				_bodyPended = true;
			}
			else
				_messageState = basic;
		}
	}
}

void HttpreqHandler::initPendingState(void)
{
	if (_messageState != basic)
		_pended = true;
	else
	 	_pended = false;
}

void HttpreqHandler::initVar(void)
{
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

	parseMethod("");
	_hasContentLength = parseContentLength();
	line = _buf.substr(CRLF2Pos + 4);
	if (!_hasContentLength)
	{
		if (!line.empty())
		{
			_event->setStatusCode(411);
			throw std::exception();
		}
	}
	else if (_hasContentLength)
	{
		if (line.length() > _contentLength)
		{
			_event->setStatusCode(413);
			throw std::exception();
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
	if (_info.method == "POST")
	{
		if (!_hasContentLength && _messageState != chunked)
		{
			_event->setStatusCode(411);
			throw std::exception();
		}
		return ;
	}
	else if (_info.method == "PATCH" || _info.method == "PUT")
	{
		_info.method = "POST";
		return ;
	}
	else if (_info.method == "GET" || _info.method == "DELETE" || _info.method == "HEAD")
		return ;
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
	int pos = 0, prevPos = 0, questionPos = 0;
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
		_info.path.erase(questionPos);
	}
}

void HttpreqHandler::checkStartLine(void)
{
	checkMethod();
	checkHttpVersion();
	checkQueryParam();
}

/* =============== constructor ================== */
HttpreqHandler::HttpreqHandler(Event *e)
	: _buf(""), _messageState(basic), _pended(false), _contentLength(0),
	_chunkedLength(0), _infoBodyIdx(0), _flag(false), _currentBodyLength(0),
	_hasContentLength(false), _headerPended(false), _bodyPended(false), _event(e)
{
	/**
	 * @ref https://nginx.org/en/docs/http/ngx_http_core_module.html#client_body_buffer_size
	 *
	 * 이 길이 넘어가면 원래 tmp파일에 저장한다고합니다.
	 * */
	this->_info.body.reserve(16384);
	//info = new HttpreqHandlerInfo();
	//_buf.clear();
	//_messageState = basic;
	//_pended = false;
	//_contentLength = 0;
	//_hasContentLength =false;
	//_headerPended =false;
	//_bodyPended =false;
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

bool HttpreqHandler::isHeaderPending(void) const { return _headerPended; }

bool HttpreqHandler::isBodyPending(void) const { return _bodyPended; }

bool HttpreqHandler::getHasSid(void) const { return (!_sidString.empty() ? true : false); }

std::string HttpreqHandler::getSidString(void) const { return _sidString;}

int HttpreqHandler::getSid(void) const { return _sid;}

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

	std::cout<<"\nQuery String"<<std::endl<<std::endl;
	std::cout<<"\033[35m"<<_info.queryParam<<std::endl<<std::endl;

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
		
			case 'a' : c = "10"; break;
			case 'b' : c = "11"; break;
			case 'c' : c = "12"; break;
			case 'd' : c = "13"; break;
			case 'e' : c = "14"; break;
			case 'f' : c = "15"; break;
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
