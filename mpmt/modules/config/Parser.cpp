#include "ConfigData.hpp"
#include "./http/HttpServerBlock.conf.hpp"
#include <fstream>
#include <string>
#include "../../lib/ft_split.hpp"
#include "http/HttpLocationBlock.conf.hpp"

class Parser
{
public:
	/**
	 * @brief for http, server, location block
	 *
	 * @param buf
	 * @param confData
	 */
	static void httpBlockParser(std::string &buf, ConfigData &confData)
	{
		ft_split s;
		if (buf.find("client_max_body_size") != std::string::npos)
		{
			s.split(buf.c_str(), ' ');
			confData.setClientMaxBodySize(std::atoi(s.get()[1].c_str()));
		}

		if (buf.find("error_page") != std::string::npos)
		{
			s.splitRemoveSemiColon(buf.c_str(), ' ');
			for (int i = 1; i < s.get().size() - 1; i++)
			{
				confData.setErrorPage(std::atoi(s.get()[i].c_str()), s.get()[s.get().size() - 1]);
			}
		}

		if (buf.find("root") != std::string::npos)
		{
			s.splitRemoveSemiColon(buf.c_str(), ' ');
			confData.setRoot(s.get()[1]);
		}

		if (buf.find("sendfile") != std::string::npos) 
		{
			s.splitRemoveSemiColon(buf.c_str(), ' ');
			if (s.get()[1] == "on")
				confData.setSendFile(true);
		}

		if (buf.find("tcp_nodelay") != std::string::npos) 
		{
			s.splitRemoveSemiColon(buf.c_str(), ' ');
			if (s.get()[1] == "off")
				confData.setTcpNoDelay(false);
		}

		if (buf.find("tcp_nopush") != std::string::npos)
		{
			s.splitRemoveSemiColon(buf.c_str(), ' ');
			if (s.get()[1] == "on")
				confData.setTcpNoPush(true);
		}

		/*
		 * getcwd -> opendir -> readdir
		 * */
		if (buf.find("autoindex") != std::string::npos)
		{
			s.split(buf.c_str(), ' ');
			if (s.get()[2] == "on;")
				confData.setAutoIndex(true);
		}
	}

	/**
	 * @brief only for server block
	 *
	 * @param buf
	 * @param confData
	 */
	static void httpServerBlockParser(std::string &buf, HttpServerBlock::HttpServerData &confData)
	{
		ft_split s;

		if (buf.find("listen") != std::string::npos)
			confData.setListen(atoi(s.splitRemoveSemiColon(buf.c_str(), ' ')[1].c_str()));

		if (buf.find("server_name") != std::string::npos)
		{
			s.splitRemoveSemiColon(buf.c_str(), ' ');
			for (int i = 1; i < s.get().size(); i++)
				confData.setServerName(s.get()[i]);
		};
	}

	/**
	 * @brief only for location block
	 *
	 * @param buf
	 * @param confData
	 */
	static void httpLocationBlockParser(std::string &buf, HttpLocationBlock::HttpLocationData &confData)
	{
		ft_split	s;

		if (buf.find("location") != std::string::npos)
		{
			s.split(buf.c_str(), ' ');
			//check exact match
			if (s.get()[1] == "=")
			{
				confData.setUriMatchMode(HttpLocationBlock::EXACT);
				confData.setUri(s.get()[2]);
			}
			else
			{
				confData.setUriMatchMode(HttpLocationBlock::PREFIX);
				confData.setUri(s.get()[1]);
			}
		}

		if (buf.find("proxy_pass") != std::string::npos)
		{
			s.splitRemoveSemiColon(buf.c_str(), ' ');
			confData.setProxyPass(s.get()[1]);
		}

		if (buf.find("fastcgi_pass") != std::string::npos)
		{
			s.splitRemoveSemiColon(buf.c_str(), ' ');
			confData.setFastcgiPass(s.get()[1]);
		}

		if (buf.find("return") != std::string::npos)
		{
			s.splitRemoveSemiColon(buf.c_str(), ' ');
			confData.setReturnStatus(std::atoi(s.get()[1].c_str()));
			confData.setRedirectUrl(s.get()[2]);
		}
	}



private:
	Parser() {}
	~Parser() {}
	void operator=(Parser &p) {}
	Parser(const Parser &p) {}
};
