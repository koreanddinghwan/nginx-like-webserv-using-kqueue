#ifndef HTTPLOCATIONDATA_HPP
# define HTTPLOCATIONDATA_HPP

#include "../data/HttpServerData.hpp"
#include <string>
#define ALLOW 1
#define DENY 0

enum METHODS {
	GET = 0,
	HEAD,
	POST,
	PUT,
	DELETE,
	MKCOL,
	COPY,
	MOVE,
	OPTIONS,
	PROPFIND,
	PROPPATCH,
	LOCK,
	UNLOCK,
	PATCH
};

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

	struct limited_methods
	{
		// GET, HEAD, POST, PUT, DELETE, MKCOL, COPY, MOVE, OPTIONS, PROPFIND, PROPPATCH, LOCK, UNLOCK, or PATCH.
		bool methods[14];
	};

private:
	enum e_uri_match_mode uri_match_mode;
	std::string uri;
	std::string proxy_pass; //default -
	std::string cgi_pass; //default -
	std::string fastcgi_pass; //default -
	int return_status;
	std::string redirect_url;
	limited_methods lim_methods;

public:
	HttpLocationData();
	~HttpLocationData();
	HttpLocationData(HttpServerData &c);
	void operator=(HttpServerData &c);

	int			getUriMatchMode(); 
	std::string	&getUri(); 
	std::string	&getProxyPass(); 
	std::string	&getCgiPass();
	std::string	&getFastcgiPass(); 
	int			getReturnStatus(); 
	std::string	&getRedirectUrl(); 

	void setUriMatchMode(enum e_uri_match_mode e); 
	void setUri(std::string u); 
	void setProxyPass(std::string p); 
	void setCgiPass(std::string c);
	void setFastcgiPass(std::string f); 
	void setReturnStatus(int r);  
	void setRedirectUrl(std::string r);
	void setLimitedMethods(std::vector<std::string> r);

	void printLocationData();
};

#endif
