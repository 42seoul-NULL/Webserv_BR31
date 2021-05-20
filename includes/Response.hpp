#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <iostream>
# include <map>
# include <ctime>
# include <sys/stat.h>
# include <dirent.h>
# include "request.hpp"
# include "resource.hpp"
# include "enums.hpp"
# include "nginx.hpp"

class Request;
class Server;
class Location;

/*
Response 책임/권한
- 
*/
class Response
{
	private:
		int		last_reponse;
		Client	*client;

		std::string raw_response;
		std::string	resource_path; // 이전 absolute path
		Location *location;
		std::string cgi_extention;
		bool	is_redirection;

	public :
		Response(void);
		virtual ~Response(void);
		
		//getter
		int		getLastResponse();
		std::string&	getRawResponse(void);
		
		//setter
		void	setLocation(Location *location);
		void	setClient(Client *client);
		void	setResourcePath(const std::string &resource_path);
		void	setCgiExtention(const std::string &cgi_extention);
		void	setIsRedirection(bool is_redirection);
		
		//response_common
		void	initResponse(void);
		void	makeResponse();
		void	makeErrorResponse(int error_num);
		//int		makeCgiResponse();

	private :
		bool	isExist(std::string &path);
		bool	isDirectory(std::string &path);
		void	makeDefaultErrorBody(std::string &body, int error);
		void	makeAutoIndexPage();
		void	setResource(int fd, e_direction direction, e_nextcall nextcall, int error_num = -1);

		int		addFirstLine(int code);
		int		addAllow();
		int		addContentLanguage();
		int		addContentLength(int size);
		int		addContentLocation();
		int		addContentType(const std::string &mime_type);
		int		addDate();
		int		addLastModified();
		int		addLocation();
		int		addRetryAfter();
		int		addServer();
		int		addWWWAuthenticate();
		void	addRawErrorBody();

		//response_get
		void	makeGetResponse();
		void	makeRedirectionResponse();

		//response_put
		// int		makePutResponse();
		// int		makeDirectoryToCreate();
		// int		createPutRequest();
		
		// char	**makeEnv();
};

#endif
