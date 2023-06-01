#include "ws_HttpUploadModule.hpp"
#include <sys/fcntl.h>


bool ws_HttpUploadModule::processEvent(Event *e)
{
	std::cout<<"uploda store: "<<e->locationData->getUploadStore()<<std::endl;
	/**
	 * @TODO get file name from request
	 * */
	std::string fileName = "testFile";

	//create file
	std::string filePath = e->locationData->getRoot() + "/" + e->locationData->getUploadStore() + "/" + fileName;

	if((e->file_fd = open(filePath.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0644)) == -1)
		return false;

	if ((fcntl(e->file_fd, F_SETFL, O_NONBLOCK)) == -1)
		return false;

	return true;
}
