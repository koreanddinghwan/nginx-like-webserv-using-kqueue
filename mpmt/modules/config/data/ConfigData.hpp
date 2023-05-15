#ifndef CONFIGDATA_HPP
# define CONFIGDATA_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>

class ConfigData
{
public:
	typedef std::map<int, std::string> errorMap;

private:
	/*
	 * http, server location block 모두에 적용
	 * */
	int client_max_body_size;

	/* error_page 404             /404.html; */
	/* error_page 500 502 503 504 /50x.html; */
	std::map<int, std::string> errorPage;

	/* location /i/ { */
	/*     root /data/w3; */
	/* } */
	std::string root; //default: html

	bool sendfile; //default : off
	bool tcp_nodelay; //default: on
	bool tcp_nopush; //default : off

	bool autoindex;
	//server_name : server
	//proxy_pass : location
	//fastcgi_pass : location


public:
	ConfigData();
	~ConfigData();
	ConfigData& operator=(ConfigData &c);
	ConfigData(ConfigData &c);

	int		getClientMaxBodySize() const;
	std::map<int, std::string> getErrorPage() const;
	std::string getRoot() const;
	bool	getSendFile() const;
	bool	getTcpNoDelay() const;
	bool	getTcpNoPush() const;
	bool	getAutoIndex() const;

	void	setClientMaxBodySize(int a);
	void	setErrorPage(int errCode, std::string html);
	void	setRoot(std::string r);
	void	setSendFile(bool a);
	void	setTcpNoDelay(bool a);
	void	setTcpNoPush(bool a);
	void	setAutoIndex(bool a);

	void	copyErrorPage(errorMap m);
	void	printConfig();
};

#endif
