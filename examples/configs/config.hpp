#ifndef CONFIG_HPP
# define CONFIG_HPP

# include <stdlib.h>
# include <string>
# include <map>
# include <list>
# include <unistd.h>
# include <fcntl.h>
# include <iostream>
# include "get_next_line.hpp"
# include <vector>

class location;

class config
{
	private :
		config(){};
		config(const config& origin){};
		config& operator=(const config& origin);

		static config*	instance;
		std::map<std::string, location> locations;

		std::string		ip;
		int				port;
		std::string		server_name;

	public	:
		virtual ~config();
		static config* getInstance();
		void	setPort(int value);
		void	setIP(const std::string& ip);
		void	setServerName(const std::string& server_name);

		const std::string& getIP();
		const std::string& getServerName();
		int				   getPort();
		std::map<std::string, location>& getLocations();

		bool				makeConfig(const char *path);
};

class location
{
	private	:
		std::string		root;
		std::list<std::string> index;
		std::list<std::string> allow_methods;
		unsigned int	client_body_buffer_size;

	public	:
		location(){};
		virtual ~location(){};
		location(const location& origin);
		location& operator=(const location& origin);

		void			setRoot(const std::string& root);
		void			setClientBodyBufferSize(unsigned int value);

		const std::string& getRoot();
		std::list<std::string>& getIndex();
		std::list<std::string>& getAllowMethods();
		const unsigned int getClientBodyBufferSize();
};
#endif