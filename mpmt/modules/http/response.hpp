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
	bool checkServerNameExistsInConfig(Event *e);

	/**
	 * setLocationData by longest prefix match
	 * */
	void setLocationData(Event *e);

	/**
	 * need to check allowed method
	 * */
	bool checkAllowMethod();

	bool checkIsNeedCgi();
	bool checkIsNeedFile();

public:
	Response();
	~Response();
};


#endif
