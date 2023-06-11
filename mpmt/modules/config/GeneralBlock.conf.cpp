#include "GeneralBlock.conf.hpp"

GeneralBlock::GeneralBlock(std::ifstream &File) 
{
	this->parse(File);
}

void GeneralBlock::parse(std::ifstream &File) 
{
	std::string buf;

	std::getline(File, buf);
	if (buf.find("worker_processes") == std::string::npos)
	{
		File.seekg(0);
		this->confData.worker_processes = 0;
		return ;
	}
	else
	{
		strSplit spl;

		strSplit::strPair splited = spl.split(buf, ' ');
		this->confData.worker_processes = std::atoi(splited.second.c_str());
	}
}
	/**
	 * @brief get generalConfig data
	 *
	 *
	 * @return generalConfigData
	 * struct generalConfig {
	 * int worker_process;
	 * };
	 */
IConfigData* GeneralBlock::getConfigData() {
	return &(this->confData);
}

GeneralBlock::~GeneralBlock() {}
