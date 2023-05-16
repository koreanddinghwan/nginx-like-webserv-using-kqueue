#ifndef HTTPDATA_HPP
# define HTTPDATA_HPP

#include "HttpConfigData.hpp"
#include "../../../interface/IBlock.hpp"
#include "../http/HttpServerBlock.conf.hpp"

/**
 * @brief HttpData
 *
 * data that only have http
 */
class HttpData: public HttpConfigData
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
