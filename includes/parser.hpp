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
	BODY_WRITING,
	RESPONSE_READY
}						e_status;

typedef enum			t_type
{
	SERVER,
	CLIENT,
	RESOURCE,
	PIPE,
	BASE
}						e_type;

class Location;
class Server;
class Response;
class Nginx;

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
		Nginx *nginx;

	public	:
		virtual ~Config();
		static Config* getInstance();
		static const int decodeMimeBase64[256];
		
		std::map<std::string, Server>& getServers();
		std::map<std::string, std::string>& getMimeType();	
		std::map<std::string, std::string>& getStatusCode();
		bool	makeConfig(const char *path);
		
		void	setNginx(Nginx *nginx);
		Nginx *getNginx();

		//for test
		void	show();
};

class Fdmanager
{
	protected :
		int				fd;
		e_type			type;
	
	public	:
		Fdmanager();
		virtual ~Fdmanager();

		void		setFd(int fd);
		int			getFd() const;
		e_type		getType() const;
		void		setType(e_type type);
};

class Resource	:	public Fdmanager
{
	private :
		int				fd_read_to;
		int				fd_write_from;

	public	:
		Resource();
		virtual ~Resource();

		void		setFdReadTo(int fd_read_to);
		void		setFdWriteFrom(int fd_write_from);

		int			getFdReadTo();
		int			getFdWriteFrom();
};

class Pipe		:	public Fdmanager
{
	public	:
		Pipe();
		Pipe(int fd);
		virtual ~Pipe();
};

class Client	:	public Fdmanager
{
	private	:
		t_status		status;
		int				server_socket_fd;
		Request			request;
		Response		response;
		unsigned long	last_request_ms;

	public	:
		Client();
		Client(int server_socket_fd, int fd);
		virtual ~Client();

		void		setServerSocketFd(int server_socket_fd);
		void		setStatus(t_status status);
		void		setRemainBody(long long remain_body);
		void		setLastRequestMs(unsigned long last_request_ms);

		int			getServerSocketFd();
		Request		&getRequest();
		Response		&getResponse();
		t_status	getStatus();
		long long	getRemainBody();
		unsigned long getLastRequestMs();
};

class Server	:	public Fdmanager
{
	private	:
		std::string		ip;
		unsigned short	port;
		std::string		server_name;
		std::map<std::string, Location> locations;

	public	:
		Server();
		Server(const Server &src);
		Server& operator=(const Server &src);
		virtual	~Server();

		void	setPort(unsigned short port);
		void	setIP(const std::string &ip);
		void	setServerName(const std::string &server_name);

		const std::string &getIP() const;
		const std::string &getServerName() const;
		unsigned short	   getPort() const;

		std::map<std::string, Location> &getLocations();
		//for test//
		void	show();
};

class Location
{
	private	:
		std::string		uri_key;
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
		void			setUriKey(const std::string &uri_key);

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
		const std::string &getUriKey();

		//for test//
		void	show();
};
#endif