#ifndef METHOD_FACTORY_HPP
# define METHOD_FACTORY_HPP

#include <string>
class MethodFactory
{
	private:
		std::string _methods[14];
	
	public:
	bool isMethod(std::string& method) const;
	int getMethodIndex(std::string method) const;
	const std::string& getMethod(int index) const;
	public:
		static MethodFactory &getInstance();

	private:
		MethodFactory();
		~MethodFactory();
		MethodFactory(const MethodFactory &other);
		MethodFactory &operator=(const MethodFactory &other);
};


#endif
