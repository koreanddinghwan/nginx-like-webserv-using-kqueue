#ifndef HTTPDATA_HPP
# define HTTPDATA_HPP

#include "HttpConfigData.hpp"

/**
 * @brief HttpData
 *
 * data that only have http
 */
class HttpServerBlock;

class HttpData: public HttpConfigData
{
	private:
		std::vector<HttpServerBlock *> httpServerBlock;

	public:
		HttpData();
		~HttpData();
		HttpData(HttpData &c);
		void operator=(HttpData &c);

		std::vector<HttpServerBlock *> &getServerBlock();
		void setServerBlock(HttpServerBlock *f);

		void printData();
};

#endif
