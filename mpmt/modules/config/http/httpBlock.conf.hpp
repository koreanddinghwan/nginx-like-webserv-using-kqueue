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
		typedef std::map<int, std::vector<HttpLocationBlock *> *> locationBlocksByPortMap;
		typedef std::map<int, std::vector<HttpLocationBlock *> *>& locationBlocksByPortMapRef;
		/**
		 * @brief location block iterator
		 * int : port number
		 * vector<HttpLocationBlock *> : location blocks
		 */
		typedef std::map<int, std::vector<HttpLocationBlock *> *>::iterator locationBlocksByPortMapIter;


private:
	HttpData confData;
	std::map<int, std::vector<HttpLocationBlock *> > locationBlocksByPort;

public:
	IConfigData* getConfigData();
	HttpBlock(std::ifstream &File);
	~HttpBlock();
	
	/*
	 * get Location Blocks By Port
	 * */
	std::map<int, std::vector<HttpLocationBlock *> *>& getLocationBlocksByPort();

	/*
	 * get Location Blocks By Port
	 * */
	std::vector<HttpLocationBlock *>* findLocationBlocksByPort(int p);


private:
	HttpBlock();
	void parse(std::ifstream &File);
};

#endif
