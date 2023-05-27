#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include "HttpreqHandler.hpp"
#include "../eventLoop/Event.hpp"
#include "../../interface/IHandler.hpp"
#include <sstream>

class Response : public IHandler
{
private:
	std::ostringstream	_buf;
	bool				_isCgi;
	bool				_isFile;
	HttpLocationData	*_locationData;

public:
	void *handle(void *data);

	/**
	 * check server name and hostname
	 * */
	bool checkServerName(Event::t_locationData *locationData);

	/**
	 * need to check allowed method
	 * */
	bool checkAllowMethod();


	/**
	 * check 
	 * */

	bool checkIsNeedCgi();
	bool checkIsNeedFile();

public:
	Response();
	~Response();
};


#endif
