#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <string>
# include <map>
# include <list>
# include "enums.hpp"

//include
# include "response.hpp"
# include "request.hpp"
# include "fdmanager.hpp"
# include "server.hpp"
//선언
class Server;

class Client	:	public Fdmanager
{
	private	:
		e_client_status		status;
		Server	*		server;
		Request			request;
		Response		response;
		unsigned long	last_request_ms;

		int				fd_read;
		int				fd_write;

		Client();
	public	:
		//생성자와 소멸자
		Client(Server* server, int fd);
		virtual ~Client();

		//setter
		void		setStatus(e_client_status status);
		void		setRemainBody(long long remain_body);
		void		setLastRequestMs(unsigned long last_request_ms);
		void		setFdRead(int fd_read);
		void		setFdWrite(int fd_write);

		//getter
		Server *	getServer();
		Request		&getRequest();
		Response		&getResponse();
		e_client_status	getStatus();
		long long	getRemainBody();
		unsigned long getLastRequestMs();
		int		getFdRead();
		int		getFdWrite();
};

#endif