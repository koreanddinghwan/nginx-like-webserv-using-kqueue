#ifndef HTTPLOCATIONBLOCK_HPP
# define HTTPLOCATIONBLOCK_HPP

#include "../../../interface/IBlock.hpp"
#include "../../../lib/ft_split.hpp"
#include "../../../lib/strSplit.hpp"
#include "../../../interface/IBlock.hpp"
#include <fstream>
#include <vector>
#include <string>
#include <iostream>

/* 
 * location ~ \.php$ { 
*   fastcgi_pass   127.0.0.1:1025; 
* } 
*
* or
*
* location / { 
*   proxy_pass      http://127.0.0.1:8080; 
* } 
* */


class HttpLocationBlock : public IBlock
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

		class HttpLocationData : public ConfigData
		{
		private:
			enum e_uri_match_mode uri_match_mode;
			std::string uri;
			std::string proxy_pass;
			std::string fastcgi_pass;
			int return_status;
			std::string redirect_url;
		public:
			HttpLocationData();
			~HttpLocationData();

			enum e_uri_match_mode getUriMatchMode(); 
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
		};

	public:
		HttpLocationData confData;


	public:
		HttpLocationBlock(std::ifstream &File, ConfigData &c);
		~HttpLocationBlock();
		HttpLocationData &getConfigData();

	private:
		void parse(std::ifstream &File);
};

#endif
