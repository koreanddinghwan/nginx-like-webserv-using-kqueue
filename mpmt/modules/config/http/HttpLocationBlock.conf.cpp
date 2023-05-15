#include "HttpLocationBlock.conf.hpp"
#include "../Parser.cpp"

HttpLocationBlock::HttpLocationBlock(std::ifstream &File, ConfigData &c) {
	static_cast<ConfigData>(this->confData) = c;
	this->parse(File);
}

HttpLocationBlock::~HttpLocationBlock() 
{}

HttpLocationBlock::HttpLocationData& HttpLocationBlock::getConfigData()
{
	return this->confData;  
}

void HttpLocationBlock::parse(std::ifstream &File)
{
	std::string	buf;
	ft_split	s;

	while (buf.find("}") == std::string::npos)
	{
		if (File.eof())
			break;
		std::getline(File, buf);

		std::cout<<"current:"<<buf<<std::endl;
		Parser::httpBlockParser(buf, this->confData);
		Parser::httpLocationBlockParser(buf, this->confData);
	}
	std::cout<<"end of location block"<<std::endl;
}


enum HttpLocationBlock::e_uri_match_mode HttpLocationBlock::HttpLocationData::getUriMatchMode() {return this->uri_match_mode;}
std::string HttpLocationBlock::HttpLocationData::getUri() {return this->uri;}
std::string HttpLocationBlock::HttpLocationData::getProxyPass() {return this->proxy_pass;}
std::string HttpLocationBlock::HttpLocationData::getFastcgiPass() {return this->fastcgi_pass;}
int HttpLocationBlock::HttpLocationData::getReturnStatus() {return this->return_status;}
std::string HttpLocationBlock::HttpLocationData::getRedirectUrl() {return this->redirect_url;}


void HttpLocationBlock::HttpLocationData::setUriMatchMode(enum HttpLocationBlock::e_uri_match_mode e) { this->uri_match_mode = e;}
void HttpLocationBlock::HttpLocationData::setUri(std::string u) { this->uri = u;}
void HttpLocationBlock::HttpLocationData::setProxyPass(std::string p) { this->proxy_pass = p;}
void HttpLocationBlock::HttpLocationData::setFastcgiPass(std::string f) { this->fastcgi_pass = f;}
void HttpLocationBlock::HttpLocationData::setReturnStatus(int r) { this->return_status = r;}
void HttpLocationBlock::HttpLocationData::setRedirectUrl(std::string r) { this->redirect_url = r;}
