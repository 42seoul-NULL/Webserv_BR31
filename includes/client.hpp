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
		Client();
	public	:
		Client(Server* server, int fd);
		virtual ~Client();

		void		setStatus(e_client_status status);
		void		setRemainBody(long long remain_body);
		void		setLastRequestMs(unsigned long last_request_ms);

		Server *	getServer();
		Request		&getRequest();
		Response		&getResponse();
		e_client_status	getStatus();
		long long	getRemainBody();
		unsigned long getLastRequestMs();
};

#endif