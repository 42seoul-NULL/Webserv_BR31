#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <iostream>
# include <vector>
# include <map>
# include "libft.hpp"
# include "enums.hpp"
# include "server.hpp"
# include "config.hpp"

# define NEEDBODY -1
# define REQUESTCOMPLETE -2

# define AUTHRIZATION "Authorization"

/*
Request 책임/권한
- Request message 파싱
- Request message의 유효성 판단
- cgi 여부 판단
- 주요 변경 header -> map< key, value > 로 변경
*/

class Client;

class Request
{
	private:
		std::string	raw_request;
		
		std::string	method;
		std::string	uri;
		std::string	http_version;
		std::map<std::string, std::string> headers;
		std::string	raw_body;
		std::string temp_body;
		bool	body_receving; //0 이면 헤더 받는 중 , 1 이면 바디 작성.
		e_body_length_info	body_length_info_type;

		Client * client;

	public:
		//constructor and destroyer
		Request(void);
		virtual ~Request(void){};

		//methods
		void	initRequest(void);
		e_request_try_make_request_return	tryMakeRequest(void);
		//setter
		void				setClient(Client *client);

		//getter
		std::string&		getRawRequest(void);
		const std::string&	getMethod(void) const;
		const std::string&	getUri(void) const;
		const std::string&	getHttpVersion(void) const;
		std::map<std::string, std::string>&	getHeaders(void) const;
		const std::string&	getRawBody(void) const;


	private:
		int		 	base64_decode(const char * text, char * dst, int numBytes);
		e_request_try_make_request_return	requestValidCheck(bool isComplete);

		void	makeStartLine(void);
		void	makeRequestHeader(void);

		void	parseMethod(void);
		void	parseUri(void);
		void	parseHttpVersion(void);

		bool	bodyCheck(void);
		bool	isComplete(void);

		bool	isValidAuthHeader(Location &loc);
		bool	isValidMethod(Location &loc);
};

#endif