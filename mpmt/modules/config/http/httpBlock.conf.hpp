#ifndef HTTPBLOCK_HPP
# define HTTPBLOCK_HPP

#include "../../../interface/IBlock.hpp"
#include "HttpServerBlock.conf.hpp"
#include <fstream>
#include <iostream>
#include <vector>

/**
 * @brief make Http block
 * Http block 1... <-> ...n server block 1... <-> ...n location block
 */
class HttpBlock: public IBlock
{
public:
	class httpData: public IConfigData
	{
		private:
			int client_max_body_size;
			bool sendfile;
			bool tcp_nopush;
			std::vector<IBlock *> httpServerBlock;

		public:
			httpData();
			~httpData();

			void	setClientMaxBodySize(int a);
			void	setSendFile(bool a);
			void	setTcpNoPush(bool a);
			int		getClientMaxBodySize() const;
			bool	getSendFile() const;
			bool 	getTcpNoPush() const;
			std::vector<IBlock *> getServerBlock();
	};


private:
	httpData confData;

public:
	HttpBlock(std::ifstream &File);
	~HttpBlock();


private:
	void parse(std::ifstream &File);
	httpData &getConfigData();
};

#endif
