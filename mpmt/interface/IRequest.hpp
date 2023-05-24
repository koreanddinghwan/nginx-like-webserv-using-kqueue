#ifndef IREQUEST_HPP
# define IREQUEST_HPP

# include <iostream>

//싱글톤으로 하면 좃댐을 감지
class IRequest {

 public:
	virtual void initRequest(std::string req) = 0;
};

#endif
