#ifndef RAIIFILE_HPP
# define RAIIFILE_HPP

#include <fstream>
#include <string>
#include "../exceptions/configParseException.hpp"

/**
 * @brief RAII scheme
 * recource allocation is initialization.
 * automatically close file with destructor
 */
class FileGuard
{
private:
	std::ifstream File;

public:
	FileGuard(std::string path) throw (configParseException);
	std::ifstream &getFile();
	~FileGuard();
};


#endif
