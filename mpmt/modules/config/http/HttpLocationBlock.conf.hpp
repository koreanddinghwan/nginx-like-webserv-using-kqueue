#ifndef HTTPLOCATIONBLOCK_HPP
# define HTTPLOCATIONBLOCK_HPP

#include "../../../interface/IBlock.hpp"
#include <fstream>

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
		struct httpLocationData : public IConfigData
		{};

	public:
		httpLocationData confData;


	public:
		HttpLocationBlock(std::ifstream &File);
		~HttpLocationBlock();
		httpLocationData &getConfigData();

	private:
		void parse(std::ifstream &File);
};

#endif
