#ifndef CONFIG_HPP
# define CONFIG_HPP

# include <stdlib.h>
# include <string>
# include <map>
# include <list>
# include <unistd.h>
# include <fcntl.h>
# include <iostream>
# include <vector>
# include <queue>

# include "libft.hpp"
# include "enums.hpp"
# include "request.hpp"
# include "response.hpp"
# include "server.hpp"

/*
Config 책임/권한
- .config를 파싱해서 데이터로 보관
- mime type map을 보유
- 응답 status code/설명 map을 보유
- base_64 디코딩 map을 보유
*/
class Config
{
	private :
		Config();
		Config(const Config &src);
		Config& operator=(const Config& src);
		bool	returnFalseWithMsg(const char *str);
		bool	isReserved(const std::string &src);

		std::map<std::string, Server> servers;
		static Config*	instance;
		std::map<std::string, std::string> mime_type;
		std::map<std::string, std::string> status_code;

	public	:
		virtual ~Config();
		static Config* getInstance();
		static const int decodeMimeBase64[256];

		std::map<std::string, Server>& getServers();
		std::map<std::string, std::string>& getMimeType();
		std::map<std::string, std::string>& getStatusCode();
		bool	makeConfig(const char *path);

		//for test
		void	show();
};



#endif
