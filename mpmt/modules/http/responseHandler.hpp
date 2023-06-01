#ifndef RESPONSEHANDLER_HPP
# define RESPONSEHANDLER_HPP

#include "../eventLoop/Event.hpp"
#include "HttpRequestInfo.hpp"
#include "../../interface/IHandler.hpp"
#include "HttpResponseInfo.hpp"
#include <fstream>

class responseHandler : public IHandler
{
public:
	responseHandler();
	responseHandler(const int &status);
	~responseHandler();

	void setRes(const int statusCode);
	void setResBody(std::string body) const;
	void setResHeader(std::string HttpV) const;
	void setResLocation(std::string location) const;
	void setResAddtionalOptions(Event *event) const;
	void setResStatusCode(const int& statusCode) const;
	void setResStatusMsg(const int& statusCode) const;
	void setResBuf() const;
	std::string& getResBody() const;
	std::string& getResHeader() const;
	std::string& getResBuf() const;
	int getResStatusCode() const;
	std::string& getResStatusMsg() const;
	
	void *handle(void *event);
private:
	responseHandler& operator=(const responseHandler &rhs);
	Response* _res;
};

#endif
