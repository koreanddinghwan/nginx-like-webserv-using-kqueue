#include "ConfigData.hpp"

ConfigData::ConfigData() :  client_max_body_size(1024), root("html")
{}
ConfigData::~ConfigData() {}
void	ConfigData::setClientMaxBodySize(int a) {this->client_max_body_size = a;}
int		ConfigData::getClientMaxBodySize() const {return this->client_max_body_size;}
