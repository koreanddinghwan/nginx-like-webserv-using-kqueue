#ifndef IHTTPBLOCK_HPP
# define IHTTPBLOCK_HPP

#include "IBlock.hpp"
#include "../modules/config/data/HttpConfigData.hpp"
#include <fstream>

class IHttpBlock : public IBlock {
public:
    virtual ~IHttpBlock() {}
};

#endif
