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
		HttpLocationBlock(std::ifstream &File) {
			this->parse(File);
		}
		~HttpLocationBlock() {}

	private:
		void parse(std::ifstream &File)
		{}
};


#endif
