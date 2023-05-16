#ifndef HTTPDATA_HPP
# define HTTPDATA_HPP

#include "HttpConfigData.hpp"
#include "../../../interface/IHttpBlock.hpp"

/**
 * @brief HttpData
 *
 * data that only have http
 */
class HttpData: public HttpConfigData
{
	private:
		std::vector<IHttpBlock *> httpServerBlock;

	public:
		HttpData();
		~HttpData();
		HttpData(HttpData &c);
		void operator=(HttpData &c);

		std::vector<IHttpBlock *> getServerBlock();
		void setServerBlock(IHttpBlock *f);

		void printData();
};

#endif
