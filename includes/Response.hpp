#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <iostream>
# include <map>
# include <ctime>
# include <sys/stat.h>
# include <dirent.h>
# include "Request.hpp"

class Request;
class Server;
class Location;

class Response
{
	private:
		int		last_reponse;
		std::string raw_response;
		void	makeDefaultBody(std::string &body, int error);
		int		makeAutoIndexPage(const Request& request, const std::string &path);
		bool	isExist(std::string &path);
		bool	isDirectory(std::string &path);
		int		checkAuth(const Request& request, Location& location);
		int		base64_decode(const char *text, char *dst, int numBytes);
		
		int		makeErrorReponse(const Request& request, Location& location, int error, int client_socket);

		int		makeFirstLine(int code);
		int		makeAllow(const Request& request, Location& location);
		int		makeContentLanguage(void);
		int		makeContentLength(int size);
		int		makeContentLocation(const Request& request, Location &location);
		int		makeContentType(const Request& request, const std::string &mime_type);
		int		makeDate(const Request& request);
		int		makeLastModified(const Request& request, Location &location);
		int		makeLocation(Location &location);
		int		makeRetryAfter();
		int		makeServer();
		int		makeWWWAuthenticate();
		int		makeGetBody(const Request& request, Location &location, int client_socket);
		int		makeRedirectionResponse(const Request& request, Location& location, int client_socket);
		int		makeGetResponse(const Request& request, Location& location, int client_socket);

		bool	isCgiExtension(const Request& request, Location& location);
		char	**makeEnv(const Request& request, Location& location);
	public:
		Response(void);
		Response(const Response& src);
		virtual ~Response(void);
		Response&	operator=(const Response& src);
		void	initResponse(void);
		int		makeResponse(const Request& request, Location& location, int client_socket);
		std::string&	getRawResponse(void);
		int		getLastResponse();
		int		makeCgiResponse(const Request& request, Location& location, int client_socket, int cgi_stdin_fd = -1);
};

#endif
