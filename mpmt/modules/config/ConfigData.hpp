#ifndef CONFIGDATA_HPP
# define CONFIGDATA_HPP

#include <string>

class ConfigData
{
private:
	int client_max_body_size;
	std::string errorPage;
	std::string root;
	//server_name : server
	//proxy_pass : location
	//fastcgi_pass : location


public:
	ConfigData();
	~ConfigData();
	void	setClientMaxBodySize(int a);
	int		getClientMaxBodySize() const;
};

#endif
