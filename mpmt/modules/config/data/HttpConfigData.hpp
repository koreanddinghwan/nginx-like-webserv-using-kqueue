#ifndef HttpCONFIGDATA_HPP
# define HttpCONFIGDATA_HPP

#include "../../../interface/IConfigData.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <map>

/**
 * @brief Configuration data for Http.
 * these data shared in http, server, location block
 */
class HttpConfigData : public IConfigData
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

	/**
	 * @brief
	 * @link https://nginx.org/en/docs/http/ngx_http_index_module.html#index
	 */
	std::vector<std::string> index;

public:
	HttpConfigData();
	~HttpConfigData();
	HttpConfigData& operator=(HttpConfigData &c);
	HttpConfigData(HttpConfigData &c);

	int		getClientMaxBodySize() const;
	std::map<int, std::string> &getErrorPage();
	std::string getRoot() const;
	bool	getSendFile() const;
	bool	getTcpNoDelay() const;
	bool	getTcpNoPush() const;
	bool	getAutoIndex() const;
	std::vector<std::string>	getIndex() const;

	void	setClientMaxBodySize(int a);
	void	setErrorPage(int errCode, std::string html);
	void	setRoot(std::string r);
	void	setSendFile(bool a);
	void	setTcpNoDelay(bool a);
	void	setTcpNoPush(bool a);
	void	setAutoIndex(bool a);
	void	setIndex(std::string);

	void	copyErrorPage(errorMap m);
	void	printConfig();
};

#endif
