#ifndef NGINX_HPP
# define NGINX_HPP

# include <iostream>
# include <string>
# include <stdlib.h>
# include <unistd.h>
# include <arpa/inet.h>
# include <sys/socket.h>
# include <sys/time.h>
# include <sys/select.h>
# include <errno.h>
# include <string>
# include <fcntl.h>
# include <sys/stat.h>
# include "parser.hpp"

class Nginx
{
	private :
		fd_set	reads;
		fd_set	writes;
		fd_set	errors;

		int		fd_max;

		std::map<int, std::map<std::string, Server> > servers;
		std::map<int, Client> clients;

		void	clear_connected_socket(int connected_socket_fd);
		const Server &getServerFromClient(int server_socket_fd, const std::string &server_name);
		Location &getPerfectLocation(int server_socket_fd, const std::string &server_name, const std::string &uri);
	
	public	:
		Nginx();
		virtual ~Nginx();

		bool	initServers(int queue_size);
		bool	run(struct timeval	timeout, unsigned int buffer_size);
};

#endif