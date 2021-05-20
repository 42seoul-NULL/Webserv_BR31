#ifndef SERVER_HPP
# define SERVER_HPP

# include <string>
# include <map>
# include <list>
# include "fdmanager.hpp"
# include "enums.hpp"

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

		Location	&getPerfectLocation(std::string &uri);

		void	setPort(unsigned short port);
		void	setIP(const std::string &ip);
		void	setServerName(const std::string &server_name);

		const std::string &getIP() const;
		const std::string &getServerName() const;
		unsigned short	   getPort() const;

		//코딩해야함
		
		std::map<std::string, Location> &getLocations();
		//for test//
		void	show();
};

#endif