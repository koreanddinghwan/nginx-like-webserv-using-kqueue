#include "EventBlock.conf.hpp"

EventBlock::EventBlock(std::ifstream &File) {
	this->parse(File);
}

void EventBlock::parse(std::ifstream &File) 
{
	int cur_offset = File.tellg();
	std::string buf;

	std::getline(File, buf);
	while (buf.length() == 0)
		std::getline(File, buf);

	if (buf.find("events") == std::string::npos)
	{
		File.seekg(cur_offset);
		this->confData.worker_connections = 1024;
		return ;
	}
	else
	{
		strSplit spl;

		std::getline(File, buf);
		strSplit::strPair splited = spl.split(buf, ' ');
		this->confData.worker_connections = std::atoi(splited.second.c_str());

		//push into next field
		while (buf.find("}") == std::string::npos)
			std::getline(File, buf);
	}
}


IConfigData* EventBlock::getConfigData() {
	return static_cast<IConfigData *>(&(this->confData));
}

EventBlock::~EventBlock() {}

EventBlock::eventConfig& EventBlock::getEventConfigData() {
	return this->confData;
}
