#ifndef PARSER_HPP
# define PARSER_HPP

# include <stdlib.h>
# include <string>
# include <map>
# include <list>
# include <unistd.h>
# include <fcntl.h>
# include <iostream>
# include <vector>
# include "../libft_cpp/libft.hpp"
# include <queue>
# include "Request.hpp"
# include "Response.hpp"

typedef enum			t_status
{
	REQUEST_RECEIVING,
	RESPONSE_READY
}						t_status;

class Location;
class Server;
class Response;

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

class Client
{
	private	:
		t_status		status;
		int				server_socket_fd;
		int				socket_fd;
		Request			request;
		Response		response;
		long long		remain_body;
		unsigned long	last_request_ms;

	public	:
		Client();
		Client(int server_socket_fd, int socket_fd);
		~Client();

		void		setSocketFd(int socket_fd);
		void		setServerSocketFd(int server_socket_fd);
		void		setStatus(t_status status);
		void		setRemainBody(long long remain_body);
		void		setLastRequestMs(unsigned long last_request_ms);

		int			getSocketFd();
		int			getServerSocketFd();
		Request		&getRequest();
		Response		&getResponse();
		t_status	getStatus();
		long long	getRemainBody();
		unsigned long getLastRequestMs();
};

class Server
{
	private	:
		std::string		ip;
		unsigned short	port;
		std::string		server_name;
		int				socket_fd;
		std::map<std::string, Location> locations;

	public	:
		Server();
		Server(const Server &src);
		Server& operator=(const Server &src);
		virtual	~Server();

		void	setPort(unsigned short port);
		void	setIP(const std::string &ip);
		void	setServerName(const std::string &server_name);
		void	setSocketFd(int socket_fd);

		const std::string &getIP() const;
		const std::string &getServerName() const;
		unsigned short	   getPort() const;
		int				   getSocketFd() const;

		std::map<std::string, Location> &getLocations();
		//for test//
		void	show();
};

class Location
{
	private	:
		std::string		root;
		std::list<std::string> index;
		std::list<std::string> allow_methods;
		std::map<int, std::string> error_pages;
		int				request_max_body_size;
		std::string		upload_path;
		bool			auto_index;
		std::string		cgi_extension;
		std::string		auth_key;

		int				redirect_return;
		std::string		redirect_addr;

	public	:
		Location();
		virtual ~Location(){};
		Location(const Location &src);
		Location& operator=(const Location &src);

		void			setRoot(const std::string &root);
		void			setRequestMaxBodySize(int request_max_body_size);
		void			setUploadPath(const std::string &upload_path);
		void			setAutoIndex(bool auto_index);
		void			setCgiExtension(const std::string &cgi_extension);
		void			setAuthKey(const std::string &auth_key);
		void			setRedirectReturn(int redirect_return);
		void			setRedirectAddr(const std::string &redirect_addr);

		const std::string &getRoot();
		std::list<std::string> &getIndex();
		std::list<std::string> &getAllowMethods();
		int getRequestMaxBodySize();
		const std::string &getUploadPath();
		bool	getAutoIndex();
		const std::string &getCgiExtension();
		const std::string &getAuthKey();
		int		getRedirectReturn();
		const std::string &getRedirectAddr();
		std::map<int, std::string> &getErrorPages();

		//for test//
		void	show();
};
#endif