#ifndef HTTPBLOCK_HPP
# define HTTPBLOCK_HPP

#include "../data/HttpData.hpp"
#include "../../../interface/IHttpBlock.hpp"
#include "HttpLocationBlock.conf.hpp"
#include "HttpServerBlock.conf.hpp"
#include "../BlockParser.hpp"
#include <fstream>
#include <iostream>
#include <vector>
#include <set>

/**
 * @brief make Http block
 * Http block 1... <-> ...n server block 1... <-> ...n location block
 */
class HttpBlock: public IHttpBlock
{
	public:
		typedef std::map<int, std::vector<HttpLocationData *> *> locationDatasByPortMap;
		typedef std::map<int, std::vector<HttpLocationData *> *>& locationDatasByPortMapRef;

		
		/**
		 * @brief location block iterator
		 * int : port number
		 * vector<HttpLocationData *> : location blocks
		 */
		typedef std::map<int, std::vector<HttpLocationData *> *>::iterator locationDatasByPortMapIter;

private:
	HttpData confData;

	/**
	 * @brief locationDatasByPort
	 * locationDatasByPort는 port를 key로, 해당 port를 가진 location block들을 value로 가지는 map이다.
	 * socket open을 위해 httpServer에서 사용된다.
	 */
	std::map<int, std::vector<HttpLocationData *>* > locationDatasByPort;

	/**
	 * @beief defaultServerData
	 * request에서 host를 찾지 못했을때, 즉, servername이 일치하지 않으면
	 * defaultServerData를 사용한다.
	 */
	HttpServerData* defaultServerData;

public:
	HttpBlock(std::ifstream &File);

	HttpData& getHttpData();
	IConfigData* getConfigData();
	~HttpBlock();
	
	/**
	 * get Location Datas By Port
	 * */
	std::map<int, std::vector<HttpLocationData *> *>& getLocationDatasByPort();


private:
	/**
	 * Do not enable default constructor
	 * */
	HttpBlock();

	/**
	 * private method for parsing
	 * */
	void parse(std::ifstream &File);
};

#endif
