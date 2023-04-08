#ifndef IBLOCK_HPP
# define IBLOCK_HPP

class IBlock
{
public:
	~IBlock() {}
	virtual void parse() = 0;
};

#endif
