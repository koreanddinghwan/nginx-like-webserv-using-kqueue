#ifndef BLOCKPARSER_HPP
# define BLOCKPARSER_HPP

#include "data/ConfigData.hpp"
#include <fstream>
#include <string>
#include "../../lib/ft_split.hpp"

class BlockParser
{
public:
	/**
	 * @brief for http, server, location block
	 *
	 * @param buf
	 * @param confData
	 */
	static void httpBlockParser(std::string &buf, ConfigData &confData);

	/**
	 * @brief only for server block
	 *
	 * @param buf
	 * @param confData
	 */
	static void httpServerBlockParser(std::string &buf, ConfigData &confData);

	/**
	 * @brief only for location block
	 *
	 * @param buf
	 * @param confData
	 */
	static void httpLocationBlockParser(std::string &buf, ConfigData &confData);

private:
	BlockParser();
	~BlockParser();
	void operator=(BlockParser &p);
	BlockParser(const BlockParser &p);
};

#endif
