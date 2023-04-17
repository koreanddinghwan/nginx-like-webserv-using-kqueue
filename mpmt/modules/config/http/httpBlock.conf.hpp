#ifndef HTTPBLOCK_HPP
# define HTTPBLOCK_HPP

#include "../../../interface/IBlock.hpp"
#include "HttpServerBlock.conf.hpp"
#include <fstream>
#include <vector>

/**
 * @brief make Http block
 * Http block 1... <-> ...n server block 1... <-> ...n location block
 */
class HttpBlock: public IBlock
{
public:
	HttpBlock(std::ifstream &File) {
		this->parse(File);
	}
	~HttpBlock() {}


private:
	std::vector<IBlock *> httpServerBlock;

	void parse(std::ifstream &File) {
		/*
		 * while 'server' directives in buf,
		 * {
		 * 		httpServerBlock.push(new httpServerBlock(File));
		 * }
		 * */
	}
};

#endif
