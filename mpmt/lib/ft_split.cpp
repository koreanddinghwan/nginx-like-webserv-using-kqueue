#include "ft_split.hpp"

std::vector<std::string> ft_split::split(char const *s, char c) 
{
	for (std::vector<std::string>::iterator it = p.begin(); it != p.end(); it++)
		(*it).clear();
	int		p_index;

	p.reserve(ft_wd_cnt((char *)s, c));
	p_index = 0;
	while (*s)
	{
		while (*s && (*s == c))
			s++;
		if (*s != 0)
			makeWord(p_index++, s, c);
		while (*s && !(*s == c))
			s++;
	}
	return (p);
}



std::vector<std::string> ft_split::get()
{
	return this->p;
}


ft_split::ft_split() {}
ft_split::~ft_split() {}


void ft_split::makeWord(int p_index, char const *s, char c)
{
	int i = 0;
	std::string str;

	while (s[i] && s[i] != c)
		str.push_back(s[i++]);
	p.push_back(str);
}

void ft_split::makeWordRemoveSemiColon(int p_index, char const *s, char c)
{
	int i = 0;
	std::string str;

	while (s[i] && s[i] != c && s[i] != ';')
		str.push_back(s[i++]);
	p.push_back(str);
}


int	ft_split::ft_wd_cnt(char *str, char c)
{
	int	count;

	count = 0;
	if (c == 0)
		return (1);
	while (*str)
	{
		while (*str && (*str == c))
			str++;
		if (*str != 0)
		{
			count++;
			while (*str && (*str != c))
				str++;
		}
	}
	return (count);
}


std::vector<std::string> ft_split::splitRemoveSemiColon(char const *s, char c)
{
	for (std::vector<std::string>::iterator it = p.begin(); it != p.end(); it++)
		(*it).clear();
	int		p_index;

	p.reserve(ft_wd_cnt((char *)s, c));
	p_index = 0;
	while (*s)
	{
		while (*s && (*s == c))
			s++;
		if (*s != 0)
			makeWordRemoveSemiColon(p_index++, s, c);
		while (*s && !(*s == c))
			s++;
	}
	return (p);
}
