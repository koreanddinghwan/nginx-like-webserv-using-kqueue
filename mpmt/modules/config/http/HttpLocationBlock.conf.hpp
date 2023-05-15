#ifndef HTTPLOCATIONBLOCK_HPP
# define HTTPLOCATIONBLOCK_HPP

#include "../data/ConfigData.hpp"
#include "../BlockParser.hpp"
#include "../../../interface/IBlock.hpp"
#include "../../../lib/ft_split.hpp"
#include "../../../lib/strSplit.hpp"
#include <fstream>
#include <vector>
#include <string>
#include <iostream>
#include "../data/HttpLocationData.hpp"

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
		HttpLocationData confData;

	public:
		HttpLocationBlock(std::ifstream &File, HttpServerData *c);
		~HttpLocationBlock();
		HttpLocationData &getConfigData();

	private:
		void parse(std::ifstream &File);
};

#endif
