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
# include "libft.hpp"
# include <queue>
# include "Request.hpp"
# include "Response.hpp"

typedef enum			t_status
{
	REQUEST_RECEIVING,
	RESPONSE_MAKING,
	FILE_READING,
	FILE_READ_DONE,
	FILE_WRITING,
	FILE_WRITE_DONE,
	RESPONSE_MAKE_DONE
}						e_status;

typedef enum			t_type
{
	SERVER,
	CLIENT,
	RESOURCE
}						e_type;

typedef enum			t_direction
{
	RAW_RES_TO_FD,
	FD_TO_RAW_RES
}						e_direction;

typedef enum			t_nextcall
{
	MAKE_ERROR_RESPONSE,
	MAKE_RESPONSE
}						e_nextcall;

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
		std::string &raw_resource;
		int			pid;
		int			response_error_num;
		Client		*client;

		e_direction	direction;
		e_nextcall	next_call;

		Resource();
	public	:
		Resource(int fd, std::string& raw_resource, Client *client, e_direction direction, e_nextcall next_call);
		bool isReady();
		void changeClientStatus();

		std::string &getRawResource();
		e_direction	getDirection();
		e_nextcall	getNextCall();

		virtual ~Resource();
};

class Client	:	public Fdmanager
{
	private	:
		t_status		status;
		Server	*		server;
		Request			request;
		Response		response;
		unsigned long	last_request_ms;
		Client();
	public	:
		Client(Server* server, int fd);
		virtual ~Client();

		void		setStatus(t_status status);
		void		setRemainBody(long long remain_body);
		void		setLastRequestMs(unsigned long last_request_ms);

		Server *	getServer();
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

		//코딩해야함
		Location	&getPerfectLocation(const std::string &uri);
		//

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
		std::map<std::string, std::string> cgi_infos;
		std::map<int, std::string> error_pages;
		int				request_max_body_size;
		std::string		upload_path;
		bool			auto_index;
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
		std::map<std::string, std::string>& getCgiInfos();
		const std::string &getAuthKey();
		int		getRedirectReturn();
		const std::string &getRedirectAddr();
		std::map<int, std::string> &getErrorPages();
		const std::string &getUriKey();

		//for test//
		void	show();
};
#endif
