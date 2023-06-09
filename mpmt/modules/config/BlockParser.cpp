#include "BlockParser.hpp"
#include "./data/HttpConfigData.hpp"
#include "http/HttpLocationBlock.conf.hpp"
#include "http/HttpServerBlock.conf.hpp"

/**
 * @brief for http, server. location block
 *
 * @param buf
 * @param confData
 */
 void BlockParser::httpBlockParser(std::string &buf, HttpConfigData &confData)
{
	ft_split s;
	if (buf.find("client_max_body_size") != std::string::npos)
	{
		s.split(buf.c_str(), ' ');
		confData.setClientMaxBodySize(std::atoi(s.get()[1].c_str()));
	}

	else if (buf.find("error_page") != std::string::npos)
	{
		s.splitRemoveSemiColon(buf.c_str(), ' ');
		for (int i = 1; i < s.get().size() - 1; i++)
		{
			confData.setErrorPage(std::atoi(s.get()[i].c_str()), s.get()[s.get().size() - 1]);
		}
	}

	else if (buf.find("root") != std::string::npos)
	{
		s.splitRemoveSemiColon(buf.c_str(), ' ');
		confData.setRoot(s.get()[1]);
	}

	else if (buf.find("sendfile") != std::string::npos) 
	{
		s.splitRemoveSemiColon(buf.c_str(), ' ');
		if (s.get()[1] == "on")
			confData.setSendFile(true);
	}

	else if (buf.find("tcp_nodelay") != std::string::npos) 
	{
		s.splitRemoveSemiColon(buf.c_str(), ' ');
		if (s.get()[1] == "off")
			confData.setTcpNoDelay(false);
	}

	else if (buf.find("tcp_nopush") != std::string::npos)
	{
		s.splitRemoveSemiColon(buf.c_str(), ' ');
		if (s.get()[1] == "on")
			confData.setTcpNoPush(true);
	}

	/*
	 * getcwd -> opendir -> readdir
	 * */
	else if (buf.find("autoindex") != std::string::npos)
	{
		s.split(buf.c_str(), ' ');
		if (s.get()[1] == "on;")
			confData.setAutoIndex(true);
		else
			confData.setAutoIndex(false);
	}

	else if (buf.find("index ") != std::string::npos)
	{
		s.splitRemoveSemiColon(buf.c_str(), ' ');
		for (int i = 1; i < s.get().size(); i++)
		{
			confData.setIndex(s.get()[i]);
		}
	}
}

/**
 * @brief only for server block
 *
 * @param buf
 * @param confData
 */
 void BlockParser::httpServerBlockParser(std::string &buf, HttpConfigData &confData)
{

	ft_split s;

	if (buf.find("listen") != std::string::npos)
		static_cast<HttpServerData&>(confData).setListen(atoi(s.splitRemoveSemiColon(buf.c_str(), ' ')[1].c_str()));

	if (buf.find("server_name") != std::string::npos)
	{
		s.splitRemoveSemiColon(buf.c_str(), ' ');
		for (int i = 1; i < s.get().size(); i++)
			static_cast<HttpServerData&>(confData).setServerName(s.get()[i]);
	};

	if (buf.find("upload_store") != std::string::npos)
	{
		s.splitRemoveSemiColon(buf.c_str(), ' ');
		static_cast<HttpServerData&>(confData).setUploadStore(s.get()[1]);
	}
}

/**
 * @brief only for location block
 *
 * @param buf
 * @param confData
 */
void BlockParser::httpLocationBlockParser(std::ifstream &File, std::string &buf, HttpConfigData &confData)
{
	ft_split	s;

	if (buf.find("location ") != std::string::npos)
	{
		s.split(buf.c_str(), ' ');

		//check exact match
		if (s.get()[1] == "=")
		{
			static_cast<HttpLocationData&>(confData).setUriMatchMode(HttpLocationData::EXACT);
			static_cast<HttpLocationData&>(confData).setUri(s.get()[2]);
		}
		else
		{
			static_cast<HttpLocationData&>(confData).setUriMatchMode(HttpLocationData::PREFIX);
			static_cast<HttpLocationData&>(confData).setUri(s.get()[1]);
		}
	}

	if (buf.find("fastcgi_pass") != std::string::npos)
	{
		s.splitRemoveSemiColon(buf.c_str(), ' ');
		static_cast<HttpLocationData&>(confData).setFastcgiPass(s.get()[1]);
	}

	if (buf.find("cgi_pass") != std::string::npos)
	{
		s.splitRemoveSemiColon(buf.c_str(), ' ');
		static_cast<HttpLocationData&>(confData).setCgiPass(s.get()[1]);
	}

	if (buf.find("proxy_pass") != std::string::npos)
	{
		s.splitRemoveSemiColon(buf.c_str(), ' ');
		static_cast<HttpLocationData&>(confData).setProxyPass(s.get()[1]);
	}

	if (buf.find("return") != std::string::npos)
	{
		s.splitRemoveSemiColon(buf.c_str(), ' ');
		static_cast<HttpLocationData&>(confData).setReturnStatus(std::atoi(s.get()[1].c_str()));
		static_cast<HttpLocationData&>(confData).setRedirectUrl(s.get()[2]);
	}

	// basically, if this directive specified, 
	// deny all methods except this directive has
	if (buf.find("limit_except ") != std::string::npos)
	{
		s.splitRemoveSemiColon(buf.c_str(), ' ');
		static_cast<HttpLocationData&>(confData).setLimitedMethods(s.get());
		while (buf.find("}") == std::string::npos)
			std::getline(File, buf);
		std::getline(File, buf);
	}
	
	if (buf.find("upload_store") != std::string::npos)
	{
		s.splitRemoveSemiColon(buf.c_str(), ' ');
		static_cast<HttpServerData&>(confData).setUploadStore(s.get()[1]);
	}
}


BlockParser::BlockParser() {}
BlockParser::~BlockParser() {}
void BlockParser::operator=(BlockParser &p) {}
BlockParser::BlockParser(const BlockParser &p) {}

