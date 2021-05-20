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
# include "config.hpp"
# include "server.hpp"
# include "client.hpp"
# include "resource.hpp"


/*
Nginx 
책임
- select()를 통해 모든 fd를 관리
> 생성자/소멸자/
*/

class Nginx 
{
	private :
		fd_set	reads;
		fd_set	writes;
		fd_set	errors;

		int		fd_max;
		std::vector<Fdmanager *> fd_pool;  /// 모든 fd pool (Server, Client, Resource가 담긴다.)

	public	:
		// 생정자 & 소멸자
		Nginx();
		virtual ~Nginx();

		// public method
		bool	initServers(); // Config 에 적혀있는 정보를 기반으로 Server 인스턴스를 직접 만들어 내서 자신의 fds 에 집어넣는다.
		bool	run(); 
		void	deleteFromFdPool(Fdmanager * fdmanager);
		void 	insertToFdpool(Fdmanager *fdmanager);
	private :
		// run()'s
		bool	isIndexOfReadFdSet(int index, fd_set &reads);
		bool	isIndexOfWriteFdSet(int index, fd_set &writes);
		void	doReadServerFd(int i);
		void	doReadClientFD(int i);
		void	doReadResourceFD(int i);
		void	doWriteClientFD(int i);
		void	doWriteResourceFD(int i);
		
};

#endif