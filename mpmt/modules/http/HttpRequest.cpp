#include "HttpRequest.hpp"

HttpRequest::HttpRequest(void) {}
HttpRequest::HttpRequest(const HttpRequest& obj) {}
HttpRequest::~HttpRequest(void) {}
HttpRequest& HttpRequest::operator=(const HttpRequest& obj) { return (*this);}

HttpRequest::HttpRequest(std::string req)  // 이걸 없애고 그냥 initRequest를 밖에서 호출하는게 맞는듯
	: _buf(req), _startLine(""), _header(""), 
	  _body(""), state(others)
{
	initRequest();
}

void HttpRequest::printReq(void)
{
	std::cout<<"\033[35m"<<"=============Request Result============"<<std::endl;
	std::cout<<"\033[35m"<<"Start Line"<<std::endl<<std::endl;
	std::cout<<"\033[35m"<<_reqInfo.method<<std::endl;
	std::cout<<"\033[35m"<<_reqInfo.path<<std::endl;
	std::cout<<"\033[35m"<<_reqInfo.httpVersion<<std::endl<<std::endl;
	std::cout<<"Header"<<std::endl<<std::endl;
	for (std::map<std::string, std::string>::iterator it = _reqHeaderMap.begin(); it != _reqHeaderMap.end(); it++)
		std::cout<<"\033[35m"<<"key:"<< it->first <<" value:"<<it->second<<std::endl;
	std::cout<<"\033[35m"<<_reqInfo.body<<std::endl;
}

void HttpRequest::initRequest(void)
{
	memset(&_reqInfo, 0, sizeof(_reqInfo));

	//if checkChunked();
	//	parseChunked();
	//else if checkSeperate();
	//	parseSepterate();
	//else // 한번에 예쁘게 들어온다고 생각하면
	parseOthers();
	printReq();
}

void HttpRequest::parseOthers(void)
{
	int pos, prevPos = 0;
	std::string line;

	while((pos = _buf.find(CRLF, prevPos)) != std::string::npos)
	{
		line = _buf.substr(prevPos, pos - prevPos);
		prevPos += line.length() + 2;
		//startLine
		if (!_startLine.compare(""))
			parseStartLine(line);
		//header
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
	_reqHeaderMap.insert(std::make_pair(key,value));
}

void HttpRequest::parseBody(void)
{
	int pos;
	std::string line;

	pos = _buf.find("\r\n\r\n");
	line = _buf.substr(pos + 4);
	/* get, delete는 body가 없긴 한데 테스터가 어떻게 넣어줄지 몰라서
	조건 method 확인하는 거 + line.empty() 수정하기 +
	congig max_client_length && header contentLength 확인하고 수정..할거 많네 */
	if (_reqInfo.method.compare("POST") && !line.empty())
	{
		//이거 {} 중괄호 포함해서 저장할지 상의하기..
		_reqInfo.body = line;
	}

}
//receiveRequest() => 객체 생성 이후에 이벤트 루프에서 호출할 함수

std::string HttpRequest::getBuf() const {
return _buf;
}

void HttpRequest::setBuf(std::string buf)
{

}