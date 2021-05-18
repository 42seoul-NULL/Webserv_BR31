#include "../includes/nginx.hpp"

Nginx::Nginx() : fd_max(-1)
{
	FT_FD_ZERO(&(this->reads));
	FT_FD_ZERO(&(this->writes));
	FT_FD_ZERO(&(this->errors));
}

Nginx::~Nginx()
{

}

void	Nginx::clear_connected_socket(int connected_socket_fd)
{
	FT_FD_CLR(connected_socket_fd, &(this->reads));
	FT_FD_CLR(connected_socket_fd, &(this->writes));
	FT_FD_CLR(connected_socket_fd, &(this->errors));
	close(connected_socket_fd);
	this->clients.erase(this->clients.find(connected_socket_fd));
	return ;
}

const Server &Nginx::getServerFromClient(int server_socket_fd, const std::string &server_name)
{
	if (this->servers[server_socket_fd].find(server_name) == this->servers[server_socket_fd].end())
		return (this->servers[server_socket_fd].begin()->second);
	return ( (this->servers[server_socket_fd][server_name]) );
}

Location &Nginx::getPerfectLocation(int server_socket_fd, const std::string &server_name, const std::string &uri)
{
	std::map<std::string, Location> *locs;


	if (this->servers[server_socket_fd].find(server_name) == this->servers[server_socket_fd].end()) // 못찾으면
		locs = &(this->servers[server_socket_fd].begin()->second.getLocations());
	else
		locs = &(this->servers[server_socket_fd][server_name].getLocations());

	Location &ret = (*locs)["/"];

	std::string key = "";
	for (std::string::const_iterator iter = uri.begin(); iter != uri.end(); iter++)
	{
		key += *iter;
		if (*iter == '/')
		{
			if (locs->find(key) == locs->end())
				return (ret);
			else
				ret = (*locs)[key];
		}
	}
	return (ret);
}

bool	Nginx::initServers(int queue_size)
{
	std::map<std::string, int> temp;

	for (std::map<std::string, Server>::iterator iter = Config::getInstance()->getServers().begin(); iter != Config::getInstance()->getServers().end(); iter++)
	{
		std::string key = iter->second.getIP() + ":" + ft_itoa(iter->second.getPort());
		if (temp.find(key) != temp.end())
		{
			std::cout << "Server " << iter->second.getServerName() << "(" << iter->second.getIP() << ":" << iter->second.getPort() << ") launched" << std::endl;
			this->servers[temp[key]][iter->second.getServerName()] = iter->second;
			continue ;
		}

		struct sockaddr_in  server_addr;
		iter->second.setSocketFd(socket(PF_INET, SOCK_STREAM, 0));

		ft_memset(&server_addr, 0, sizeof(server_addr));
		server_addr.sin_family = AF_INET;
		server_addr.sin_addr.s_addr = inet_addr(iter->second.getIP().c_str());
		server_addr.sin_port = ft_htons(iter->second.getPort());

		if (bind(iter->second.getSocketFd(), (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
		{
			std::cerr << "bind() error" << std::endl;
			throw strerror(errno);
		}
		if (listen(iter->second.getSocketFd(), queue_size) == -1)
			throw strerror(errno);

		std::cout << "Server " << iter->second.getServerName() << "(" << iter->second.getIP() << ":" << iter->second.getPort() << ") started" << std::endl;

		// 서버소켓은 read 와 error 만 검사.
		FT_FD_SET(iter->second.getSocketFd(), &(this->reads));
		FT_FD_SET(iter->second.getSocketFd(), &(this->errors));

		this->servers[iter->second.getSocketFd()][iter->second.getServerName()] = iter->second;
		temp[key] = iter->second.getSocketFd();

		if (this->fd_max < iter->second.getSocketFd())
			this->fd_max = iter->second.getSocketFd();
	}
	return (true);
}

bool	Nginx::run(struct timeval	timeout, unsigned int buffer_size)
{
	fd_set	cpy_reads;
	fd_set	cpy_writes;
	fd_set	cpy_errors;

	struct sockaddr_in  client_addr;
	socklen_t			addr_size = sizeof(client_addr);
	int		fd_num;
	char	buf[buffer_size + 1];

	unsigned long	timeout_ms = (timeout.tv_sec * 1000) + (timeout.tv_usec / 1000);	

	while (1)
	{
		usleep(5); // cpu 점유가 100% 까지 올라가는 것을 막기 위해서

		cpy_reads = this->reads;
		cpy_writes = this->writes;
		cpy_errors = this->errors;

		if ( (fd_num = select(this->fd_max + 1, &cpy_reads, &cpy_writes, &cpy_errors, &timeout)) == -1)
			throw strerror(errno);

		if (fd_num == 0)
			continue ;
		
		for (int i = 0; i <= this->fd_max; i++)
		{
			if (FT_FD_ISSET(i, &cpy_reads))
			{
				if (this->servers.count(i) == 1) // server socket event
				{
					std::cout << "\033[32m server connection called \033[0m" << std::endl;
					int client_socket = accept(i, (struct sockaddr*)&client_addr, &addr_size);
					fcntl(client_socket, F_SETFL, O_NONBLOCK);

					FT_FD_SET(client_socket, &(this->reads));
					FT_FD_SET(client_socket, &(this->writes));
					FT_FD_SET(client_socket, &(this->errors));
					if (this->fd_max < client_socket)
						this->fd_max = client_socket;

					this->clients[client_socket].setServerSocketFd(i);
					this->clients[client_socket].setSocketFd(client_socket);
					this->clients[client_socket].setLastRequestMs(ft_get_time());

					std::cout << "connected client : " << client_socket << std::endl;
				}
				else	// client socket event
				{
					std::cout << "\033[34m client socket read called \033[0m" << std::endl;

					int		len;
					bool	is_readable = false;

					this->clients[i].setLastRequestMs(ft_get_time());
					while ( (len = read(i, buf, buffer_size)) > 0 )
					{
						is_readable = true;
						buf[len] = 0;
						this->clients[i].getRequest().getRawRequest() += buf; // 무조건 더한다. (다음 리퀘스트가 미리 와있을 수 있다.)
					}
					if (this->clients[i].getStatus() == REQUEST_RECEIVING && clients[i].getRequest().tryMakeRequest() == true)
					{
						std::string _server_name = clients[i].getRequest().getHost();
						size_t idx;
						if ((idx = _server_name.find(':') ) != std::string::npos)
							_server_name = _server_name.substr(0, idx);
						this->clients[i].getResponse().makeResponse(this->clients[i].getRequest(), getPerfectLocation(this->clients[i].getServerSocketFd(), _server_name, this->clients[i].getRequest().getUri() ));
						this->clients[i].getRequest().initRequest();
						this->clients[i].setStatus(RESPONSE_READY);
						std::cout << clients[i].getResponse().getRawResponse() << std::endl;
					}
					if (is_readable == false)
					{
						clear_connected_socket(i);
						if (len == 0)
							std::cout << "disconnected : " << i << " in Server" << std::endl;
						else
							std::cout << "error client : " << i << " in Server" << std::endl;
					}
				}
			}
			else if (FT_FD_ISSET(i, &cpy_writes))
			{
				//always client socket event
				if (ft_get_time() - this->clients[i].getLastRequestMs() > timeout_ms)
				{
					std::cout << "\033[34m connection timeout \033[0m" << std::endl;
					clear_connected_socket(i);
					continue ;
				}
				if (this->clients[i].getStatus() == RESPONSE_READY)
				{
					write(i, this->clients[i].getResponse().getRawResponse().c_str(), this->clients[i].getResponse().getRawResponse().size());
					if (this->clients[i].getResponse().getLastResponse() == 401) // authentication 을 요구할경우 즉시 끊어준다.
						clear_connected_socket(i);
					else
					{
						this->clients[i].getResponse().initResponse();
						this->clients[i].setStatus(REQUEST_RECEIVING);
					}
				}
			}
			else if (FT_FD_ISSET(i, &cpy_errors))
			{
				if (this->servers.count(i) == 1) // server socket error
				{
					FT_FD_CLR(i, &reads);
					FT_FD_CLR(i, &errors);
					close(i);
					std::cout << "server " << i << " error!!" << std::endl;
					this->servers.erase(this->servers.find(i));
				}
				else // client socket error
				{
					clear_connected_socket(i);
					std::cout << "error client disconnected : " << i << std::endl;
				}
			}
		}
	}
	return (true);
}