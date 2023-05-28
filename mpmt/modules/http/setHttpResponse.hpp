#ifndef SETHTTPRESPONSE_HPP
# define SETHTTPRESPONSE_HPP

#include "../eventLoop/Event.hpp"
#include "HttpReqHandler.hpp"
#include "../../factory/MethodFactory.hpp"
#include <exception>

void setHttpResponse(Event *e);
int getLongestPrefixMatchScore(const std::string& location, const std::string& requestPath);


#endif
