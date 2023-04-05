#ifndef ISOCKETCONFIG_HPP
# define ISOCKETCONFIG_HPP

template <class initType>
class ISocketConfig
{
public:
	virtual void init(initType arg) = 0;
	virtual ~ISocketConfig() {}
};

#endif
