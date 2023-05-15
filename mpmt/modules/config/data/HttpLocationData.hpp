#ifndef HTTPLOCATIONDATA_HPP
# define HTTPLOCATIONDATA_HPP

#include "../data/HttpServerData.hpp"
#include <string>

class HttpLocationData : public HttpServerData
{
public:
	/*
	 * check docs/configs.md/#default error pages
	 * */
	enum e_uri_match_mode 
	{
		PREFIX = 0,
		EXACT, // = 
		/* REGEX, */
		/* PREFERENTIAL */
	};

private:
	enum e_uri_match_mode uri_match_mode;
	std::string uri;
	std::string proxy_pass; //default -
	std::string fastcgi_pass; //default -
	int return_status;
	std::string redirect_url;
public:
	HttpLocationData();
	~HttpLocationData();
	HttpLocationData(HttpServerData &c);
	void operator=(HttpServerData &c);

	int getUriMatchMode(); 
	std::string getUri(); 
	std::string getProxyPass(); 
	std::string getFastcgiPass(); 
	int getReturnStatus(); 
	std::string getRedirectUrl(); 

	void setUriMatchMode(enum e_uri_match_mode e); 
	void setUri(std::string u); 
	void setProxyPass(std::string p); 
	void setFastcgiPass(std::string f); 
	void setReturnStatus(int r);  
	void setRedirectUrl(std::string r);

	void printLocationData();
};

#endif
