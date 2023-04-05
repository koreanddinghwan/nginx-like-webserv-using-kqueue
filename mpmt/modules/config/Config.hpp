#ifndef CONFIG_HPP
# define CONFIG_HPP

#include "../../interface/IModule.hpp"
#include <stdexcept>

class Config: public IModule
{
private:
	const char *ConfigPath;

public:
	Config(const char *configPath) : ConfigPath(configPath) {}

	/**
	 * @brief do configuration
	 *
	 * @throw std::runtime_error
	 */
	void run() throw(std::runtime_error)
	{}

	


	~Config() {}
};

#endif
