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
# include <vector>
# include "parser.hpp"

class Nginx
{
	private :
		fd_set	reads;
		fd_set	writes;
		fd_set	errors;

		int		fd_max;

		std::vector<Fdmanager *> fds;

	public	:
		Nginx();
		virtual ~Nginx();

		bool	initServers(int queue_size);
		bool	run(struct timeval	timeout, unsigned int buffer_size);

		void	clear_connected_socket(Fdmanager * fdmanager);
		void 	insert_pull(Fdmanager *fdmanager);
		Location &getPerfectLocation(int server_socket_fd, Request		&request);

		std::vector<Fdmanager *>  & getFds();
		
};

#endif