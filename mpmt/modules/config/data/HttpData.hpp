#ifndef HTTPDATA_HPP
# define HTTPDATA_HPP

#include "ConfigData.hpp"
#include "../../../interface/IBlock.hpp"
#include "../http/HttpServerBlock.conf.hpp"

class HttpData: public ConfigData
{
	private:
		std::vector<IBlock *> httpServerBlock;

	public:
		HttpData();
		~HttpData();
		HttpData(HttpData &c);
		void operator=(HttpData &c);

		std::vector<IBlock *> getServerBlock();
		void setServerBlock(HttpServerBlock *f);

		void printData();
};

#endif
