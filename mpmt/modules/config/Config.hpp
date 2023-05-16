#ifndef SINGLETONCONFIG_HPP
# define SINGLETONCONFIG_HPP

#include "http/httpBlock.conf.hpp"
#include "../../exceptions/configParseException.hpp"
#include "../../interface/IHttpBlock.hpp"
#include "../../interface/IGeneralBlock.hpp"
#include "../../interface/IEventBlock.hpp"
#include "EventBlock.conf.hpp"
#include "GeneralBlock.conf.hpp"
#include "data/HttpLocationData.hpp"
#include "data/HttpServerData.hpp"
#include "http/HttpServerBlock.conf.hpp"
#include "../../interface/IConfig.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include "../../lib/FileGuard.hpp"

/**
 * @brief config module, singleton
 *
 * 설정파일을 읽어 블록단위로 설정합니다.
 * general, event, Http, smtp, ftp, stream 총 6개의 블록이 있으며, 필수 구현부는 general, event, Http입니다.
 */
class Config : public IConfig
{
	public:
		/**
		 * @brief singleton config 객체 얻기
		 *
		 * @return signleton 객체
		 */
		static Config& getInstance();

		/**
		 * @brief config file 파싱해서 block별로 정리
		 *
		 * @param path
		 */
		void initConfig(std::string path) throw (configParseException);

		IBlock **getBlocks();
		IGeneralBlock *getGeneralBlock();
		IEventBlock *getEventBlock();
		IHttpBlock *getHTTPBlock();


		void printConfigData();

		/*
		 * 아래 3개는 구현부가 아닙니다.
		 * */
		IBlock *getSMTPBlock();
		IBlock *getFTPBlock();
		IBlock *getSTREAMBlock();

	private:
		Config();
		~Config();
		Config(const Config&);
		Config& operator=(const Config&);
	private:
		std::string		buf;
		IBlock			*blocks[6];
};


#endif
