#ifndef RESPONSEHANDLER_HPP
# define RESPONSEHANDLER_HPP

#include "HttpRequestInfo.hpp"
#include "../../interface/IHandler.hpp"
#incldue "HttpResponseInfo.hpp"
class responseHandler : public IHandler
{
public:
	responseHandler() {}
	~responseHandler() {}

	void *handler(void *httpBlock) {
		Response::Res = new Response();
		const fullPath = longestPrefixMatching(req->host, req->path, httpBlock);
		const statusCode = 코드판별식  
		Res->buf << reqInfo->HttpVersion << " " << Res->statusCode << " " << statusCodeMsg << "\r\n";
		if (Res->body) {
			Res->buf << "Content-Length: " << Res->body << "\r\n" 
			Res->buf << "Content-Type: " << "text/html" 
			Res->Buf << "\r\n\r\n"
			Res->Buf << Res->body << "\r\n"
		}
		else 
			Res->Buf << "\r\n\r\n"

		
	
		//->main obj access->orgin path type check function call(host)->return path;
		//->set condition -> return to main loop with condition -> file read | cgi | make full obj for send to client
		//needed / check req firstLine. http Ver, path, origin. / set Header function to string, throw exception with status code /


	}
	void *longestPrefixMatching(host, path, blocks) {
		const score;
		const rootPath;
		const uriPath;
		const path;
		const old_score;
		const tmp;

		tmp = blocks;
		path = "";
		old_score = path.length()
		while (tmp) {
			rootPathLen = tmp->serverBlock->hostname.find(host);
			if (rootPath != string::nPos) {
				uriPathLen = tmp->serverBlock->location->path.find(path);
				if (uriPathLen != string::nPos && uriPathLen == 0) {
					score = tmp->serverBlock->location->path.length() - path.length();
					path = score < old_score ? tmp->serverBlock->location->path : path;
					old_score = score;
				} else {
					tmp->serverBlock->location++;
				}
			} else {
				tmp->serverBlcok++;
			}
		}
		return (!path.empty() ? path : blocks->defaultPath)
	}
	void *getLocationPath(void *host, void *blocks) {
		const target;
		const tmp = blocks;
		while (tmp) {
			if (tmp->locationRootPath === host) {
				target = tmp->locationRootPath;
				break ;
			}
			tmp = tmp->next;
		}
		if (!target) {
			target = blocks->defaultPath;
		}
	}
};

#endif
