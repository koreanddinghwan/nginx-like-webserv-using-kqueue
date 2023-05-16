#ifndef WEBSERV_HPP
# define WEBSERV_HPP

#include "../../interface/IWebServ.hpp"
#include "WebservManager.hpp"
#include "../../exceptions/configParseException.hpp"
#include <exception>
#include <iostream>
#include <stdexcept>

/**
 * @brief singleton
 */
class WebServ : public IWebServ
{
	public:
		static WebServ& getInstance();
		/**
		 * @brief webserve manager를 기반으로 webserv를 구동한다.
		 *
		 * @param ac
		 * @param av
		 */
		void run(int ac, char **av) const;

	private:
		WebServ();
};

#endif
