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
	 * */
	std::map<int, std::vector<HttpLocationData *>* > locationDatasByPort;

public:
	IConfigData* getConfigData();
	HttpBlock(std::ifstream &File);
	~HttpBlock();
	
	/*
	 * get Location Datas By Port
	 * */
	std::map<int, std::vector<HttpLocationData *> *>& getLocationDatasByPort();

	/*
	 * find Location Datas By Port
	 * */
	std::vector<HttpLocationData *>* findLocationDatasByPort(int p);


private:
	HttpBlock();
	void parse(std::ifstream &File);
};

#endif
