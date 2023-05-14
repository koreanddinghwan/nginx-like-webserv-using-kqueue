#ifndef FT_SPLIT
# define FT_SPLIT

#include <iostream>
#include <vector>
#include <string>

class ft_split
{
private:
	std::vector<std::string> p;

public:

typedef typename std::vector<std::string> splited;

	std::vector<std::string> get();
	std::vector<std::string> split(char const *s, char c);
	std::vector<std::string> splitRemoveSemiColon(char const *s, char c);
	ft_split();
	~ft_split();

private:
	void makeWord(int p_index, char const *s, char c);
	void makeWordRemoveSemiColon(int p_index, char const *s, char c);
	int	ft_wd_cnt(char *str, char c);
};

#endif
