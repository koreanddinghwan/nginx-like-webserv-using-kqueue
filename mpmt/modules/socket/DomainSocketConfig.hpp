#ifndef DOMAINSOCKETCONFIG_HPP
# define DOMAINSOCKETCONFIG_HPP

#include "ISocketConfig.hpp"

template <class initType>
class DomainSocketConfig  : ISocketConfig<initType>
{
	DomainSocketConfig<initType>() {}
	~DomainSocketConfig<initType>() {}

	void init(initType arg) {

	}
};

#endif
