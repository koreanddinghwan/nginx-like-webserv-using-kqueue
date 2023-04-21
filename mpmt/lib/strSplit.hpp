#ifndef STRSPLIT_HPP
# define STRSPLIT_HPP

#include <string>
#include <utility>

class strSplit
{
private:
	std::pair<std::string, std::string> splited;

public:
	typedef typename std::pair<std::string, std::string> strPair;

public:
	strSplit();
	/**
	 * @brief delimeter를 기준으로 문자열을 나눕니다. stack에 선언되어있어야합니다.
	 *
	 * @param std::string : split할 문자열
	 * @param del : 구분자
	 *
	 * @return 
	 */
	std::pair<std::string, std::string> split(std::string str, const char del);
	~strSplit();
};

#endif
