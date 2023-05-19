#include "HttpRequest.hpp"
#include <string>
#include <utility>

HttpRequest::HttpRequest(const HttpRequest& obj) {}
HttpRequest::~HttpRequest(void) {}
HttpRequest& HttpRequest::operator=(const HttpRequest& obj) { return (*this);}


/* 
	리스닝 소켓 감지 이후 HttpRequest객체 생성
*/
HttpRequest::HttpRequest(void)
: _buf(""), _messageState(basic) 
{ 
	memset(&_reqInfo, 0, sizeof(_reqInfo));
	//initRequest();
}

/*
	근데 그러면 객체 생성할 때 req가 들어올 일이 없음
	메인 테스트용..
*/
HttpRequest::HttpRequest(std::string req)  // 이걸 없애고 그냥 initRequest를 밖에서 호출하는게 맞는듯
	: _buf(""), _messageState(basic)
{
	//map이 들어있는 구조체를 멤셋으로 초기화해버리면 좃댄다...
	//memset(&_reqInfo, 0, sizeof(_reqInfo));
	initRequest(req);
}

void HttpRequest::printReq(void)
{
	std::cout<<"\033[35m"<<"=============Request Result============"<<std::endl;
	std::cout<<"\033[35m"<<"Start Line"<<std::endl<<std::endl;
	std::cout<<"\033[35m"<<_reqInfo.method<<std::endl;
	std::cout<<"\033[35m"<<_reqInfo.path<<std::endl;
	std::cout<<"\033[35m"<<_reqInfo.httpVersion<<std::endl<<std::endl;
	std::cout<<"Header"<<std::endl<<std::endl;
	for (std::map<std::string, std::string>::iterator it = _reqInfo.reqHeaderMap.begin(); it != _reqInfo.reqHeaderMap.end(); it++)
		std::cout<<"\033[35m"<<"key:"<< it->first <<" value:"<<it->second<<std::endl;
	std::cout<<"\033[35m"<<_reqInfo.body<<std::endl;
	std::cout<<_messageState<<std::endl;
	std::cout<<"\033[35m"<<"=============Request Result End============"<<std::endl;
}

/*
	얘가 아마 밖에서 처음 호출 될 애인 듯 싶은디 
	아마 처음 한 번
*/
void HttpRequest::initRequest(std::string req)
{
	appendBuf(req);
	// chunked, basic,seperate 메세지 형식 구분하는 함수
	initMessegeState();
	//if (_messageState == chunked)
	//if checkChunked();
	//	parseChunked();
	//else if checkSeperate();
	//	parseSepterate();
	//else // 한번에 예쁘게 들어온다고 생각하면
	parseBasic();
	printReq();
}

void HttpRequest::appendBuf(std::string req)
{
	_buf.append(req);
}

void HttpRequest::initMessegeState(void)
{
	int pos;

	pos = _buf.find(CRLF2);
	if (pos == std::string::npos)
		_messageState = seperate;
	// 처음에 CRLF가 2개 들어왔으면
	else
	{
		pos = _buf.find("Transfer-Encoding: chunked");
		if (pos != std::string::npos) // chuked는 septerate하게 안들어온다고 가정, 나중에 수정해야될수도
			_messageState = chunked;
		else
		{
			if (checkSeperate(pos))
				_messageState = seperate;
			else
				_messageState = basic;
		}
	}
}

void HttpRequest::parseMethod(void)
{
	int prevPos, pos;

	pos = _buf.find(" ");
	_method = _buf.substr(0, pos);
}

void HttpRequest::parseContentLength(void)
{
	int prevPos, pos;
	char *endptr = NULL;
	std::string lengthStr;

	prevPos = _buf.find("Content-Length: ");
	pos = _buf.find(CRLF, prevPos);
	//15 == Content-Length길이
	lengthStr = _buf.substr(prevPos + 15, pos - prevPos - 15);
	_reqInfo.ContentLength = std::strtod(lengthStr.c_str(), &endptr);
	//_reqInfo.ContentLength = std::stoi(lengthStr);
}

bool HttpRequest::checkSeperate(int crlf2Pos)
{
	std::string line;

	parseMethod();
	//post가 아니면 contentlength가 없으니까 basic
	if (_method.compare("POST"))
		return false;
	parseContentLength();
	line = _buf.substr(crlf2Pos + 4);
	if (line.length() != _reqInfo.ContentLength)
		return true;
	return false;
}

void HttpRequest::parseBasic(void)
{
	int pos, prevPos = 0;
	std::string line;

	while((pos = _buf.find(CRLF, prevPos)) != std::string::npos)
	{
		line = _buf.substr(prevPos, pos - prevPos);
		prevPos += line.length() + 2;
		//startLine
		if (_reqInfo.method.empty())
			parseStartLine(line);
		//header
		else 
			parseHeader(line);
	}
	//body
	parseBody();
	

}

void HttpRequest::parseStartLine(std::string line) 
{
	int pos, prevPos = 0;
	std::string subLine;

	while ((pos = line.find(" ", prevPos)) != std::string::npos)
	{
		subLine = line.substr(prevPos, pos - prevPos);
		if (_reqInfo.method.empty())
			_reqInfo.method = subLine;
		else if (_reqInfo.path.empty())
		{
			_reqInfo.path = subLine;
			_reqInfo.httpVersion = line.substr(pos + subLine.length());
		}
		prevPos += subLine.length() + 1;
	}
}

void HttpRequest::parseHeader(std::string line){
	int pos;
	std::string key, value;

	pos = line.find(": ");
	if (pos == std::string::npos)
		return ;// 종료, header 끝나고 body로 넘어가야됨
	key = line.substr(0, pos);
	value = line.substr(pos + 2);
	_reqInfo.reqHeaderMap.insert(std::make_pair(key,value));
}

void HttpRequest::parseBody(void)
{
	int pos;
	std::string line;

	if (_reqInfo.method.compare("POST")) 
		return ;
	pos = _buf.find(CRLF2);
	line = _buf.substr(pos + 4);
	/* get, delete는 body가 없긴 한데 테스터가 어떻게 넣어줄지 몰라서
	조건 method 확인하는 거 + line.empty() 수정하기 +
	congig max_client_length && header contentLength 확인하고 수정..할거 많네 */
	//if (_reqInfo.method.compare("POST") && !line.empty())
	//{
		//이거 {} 중괄호 포함해서 저장할지 상의하기..
		_reqInfo.body = line;
	//}

}
//receiveRequest() => 객체 생성 이후에 이벤트 루프에서 호출할 함수

std::string HttpRequest::getBuf() const {
return _buf;
}

void HttpRequest::setBuf(std::string buf)
{

}