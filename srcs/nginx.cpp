#include "../includes/nginx.hpp"

Nginx::Nginx() : fd_max(-1)
{
	this->fds.resize(1024, NULL);
	FT_FD_ZERO(&(this->reads));
	FT_FD_ZERO(&(this->writes));
	FT_FD_ZERO(&(this->errors));
}

Nginx::~Nginx()
{

}

std::vector<Fdmanager *>  & Nginx::getFds()
{
	return (this->fds);
}

void	Nginx::clear_connected_socket(Fdmanager * fdmanager)
{
	FT_FD_CLR(fdmanager->getFd(), &(this->reads));
	FT_FD_CLR(fdmanager->getFd(), &(this->writes));
	FT_FD_CLR(fdmanager->getFd(), &(this->errors));
	close(fdmanager->getFd());
	this->fds[fdmanager->getFd()] = NULL;
	delete fdmanager;
	return ;
}

void	Nginx::insert_pull(Fdmanager *fdmanager) // 이미 new 가 되어 들어온다.
{
	int fd = fdmanager->getFd();

	switch (fdmanager->getType())
	{
	case CLIENT:
	{
		fcntl(fd, F_SETFL, O_NONBLOCK);
		FT_FD_SET(fd, &(this->reads));
		FT_FD_SET(fd, &(this->writes));
		FT_FD_SET(fd, &(this->errors));
		if (this->fd_max < fd)
			this->fd_max = fd;
		this->fds[fd] = fdmanager;
		break ;
	}
	case RESOURCE:
	{
		Resource *res = dynamic_cast<Resource *>(fdmanager);
		if (res->getFdReadTo() != -1) // read해서 어딘가의 client 의 raw 에 적어야한다.
		{
			FT_FD_SET(fd, &(this->reads));
			FT_FD_SET(fd, &(this->errors));
		}
		else if (res->getFdWriteFrom() != -1) // 어딘가의 client 의 raw에서 읽어서 fd에 write 해야 한다.
		{
			FT_FD_SET(fd, &(this->writes));
			FT_FD_SET(fd, &(this->errors));
		}
		if (this->fd_max < fd)
			this->fd_max = fd;
		this->fds[fd] = fdmanager;
		break ;
	}
	default:
		break;
	}
}

Location &Nginx::getPerfectLocation(int server_socket_fd, Request & request)
{
	Server *target_server = dynamic_cast<Server *>(this->fds[server_socket_fd]);
	std::map<std::string, Location> *locs = &(target_server->getLocations());

	Location *ret = &((*locs)["/"]);
	std::string key = "";
	for (std::string::const_iterator iter = request.getUri().begin(); iter != request.getUri().end(); iter++)
	{
		key += *iter;
		if (*iter == '/')
		{
			if (locs->find(key) == locs->end())
				return (*ret);
			else
				ret = &((*locs)[key]);
		}
	}
	if ( *(--key.end()) != '/') // '/'로 끝나지 않았고
	{
		key += '/';
		if (locs->find(key) != locs->end())
		{
			request.setUri(key);
			return (*locs)[key];
		}
	}
	return (*ret);
}

bool	Nginx::initServers(int queue_size)
{
	for (std::map<std::string, Server>::iterator iter = Config::getInstance()->getServers().begin(); iter != Config::getInstance()->getServers().end(); iter++)
	{
		struct sockaddr_in  server_addr;
		iter->second.setFd(socket(PF_INET, SOCK_STREAM, 0));
		int option = 1;
		setsockopt(iter->second.getFd(), SOL_SOCKET, SO_REUSEADDR, &option, sizeof(int));

		ft_memset(&server_addr, 0, sizeof(server_addr));
		server_addr.sin_family = AF_INET;
		server_addr.sin_addr.s_addr = inet_addr(iter->second.getIP().c_str());
		server_addr.sin_port = ft_htons(iter->second.getPort());

		if (bind(iter->second.getFd(), (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
		{
			std::cerr << "bind() error" << std::endl;
			throw strerror(errno);
		}
		if (listen(iter->second.getFd(), queue_size) == -1)
			throw strerror(errno);

		std::cout << "Server " << iter->second.getServerName() << "(" << iter->second.getIP() << ":" << iter->second.getPort() << ") started" << std::endl;

		// 서버소켓은 read 와 error 만 검사.
		FT_FD_SET(iter->second.getFd(), &(this->reads));
		FT_FD_SET(iter->second.getFd(), &(this->errors));

		this->fds[iter->second.getFd()] = &(iter->second);

		if (this->fd_max < iter->second.getFd())
			this->fd_max = iter->second.getFd();
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
				switch (this->fds[i]->getType())
				{
				case SERVER:
				{
					std::cout << "\033[32m server connection called \033[0m" << std::endl;
					int client_socket = accept(i, (struct sockaddr*)&client_addr, &addr_size);

					Client* temp_client = new Client();
					temp_client->setServerSocketFd(i);
					temp_client->setFd(client_socket);
					temp_client->setLastRequestMs(ft_get_time());

					insert_pull(temp_client);
					std::cout << "connected client : " << client_socket << std::endl;
					break;
				}
				case CLIENT:
				{
					std::cout << "\033[34m client socket read called \033[0m" << std::endl;

					int		len;
					Client *client = dynamic_cast<Client *>(this->fds[i]);

					client->setLastRequestMs(ft_get_time());
					len = read(i, buf, buffer_size);
					if (len <= 0)
					{
						std::cout << "disconnected : " << i << " in Server" << std::endl;
						clear_connected_socket(client);
					}
					else
					{
						buf[len] = 0;
						client->getRequest().getRawRequest() += buf; // 무조건 더한다. (다음 리퀘스트가 미리 와있을 수 있다.)
						std::cout << buf; ///////////////////////////////

						if ((client->getStatus() == REQUEST_RECEIVING) && (client->getRequest().tryMakeRequest() == true))
						{
							client->getResponse().makeResponse(client->getRequest(), getPerfectLocation(client->getServerSocketFd(), client->getRequest() ), i);
							client->getRequest().initRequest();
						}
					}
					break ;
				}
				case RESOURCE:  // read 다 -> 적혀있는 타겟 클라이언트 response 에 적어주면 된다.
				{
					int len;
					Resource *resource = dynamic_cast<Resource *>(this->fds[i]);
					Client *client = dynamic_cast<Client *>(this->fds[resource->getFdReadTo()]);
					len = read(i, buf, buffer_size);
					buf[len] = 0;
					client->getResponse().getRawResponse() += buf;
					if (len < static_cast<int>(buffer_size)) // 다읽었다.
					{
						client->setStatus(RESPONSE_READY);
						clear_connected_socket(resource);
					}
				}
				default:
					break;
				}
			}
			else if (FT_FD_ISSET(i, &cpy_writes))
			{
				switch (this->fds[i]->getType())
				{
				case CLIENT:
				{
					Client *client = dynamic_cast<Client *>(this->fds[i]);
					if (client->getStatus() == RESPONSE_READY)
					{
						write(i, client->getResponse().getRawResponse().c_str(), client->getResponse().getRawResponse().size());
						std::cout << std::endl; /////////////////////////////////////
						std::cout << client->getResponse().getRawResponse() << std::endl; //////////////////////////////
						if (client->getResponse().getLastResponse() == 401) // authentication 을 요구할경우 즉시 끊어준다.
							clear_connected_socket(client);
						else
						{
							client->getResponse().initResponse();
							client->setStatus(REQUEST_RECEIVING);
						}
					}
					break;
				}
				case RESOURCE:
				{
					Resource *res = dynamic_cast<Resource *>(this->fds[i]);
					Client *cli = dynamic_cast<Client *>(this->fds[res->getFdWriteFrom()]);
					write(res->getFd(), cli->getRequest().getRawBody().c_str(), cli->getRequest().getRawBody().size());
					clear_connected_socket(res);
					cli->setStatus(RESPONSE_READY);

					break ;
				}
				default:
					break;
				}
			}
			else if (FT_FD_ISSET(i, &cpy_errors))
			{
				clear_connected_socket(this->fds[i]);
			}
		}
	}
	return (true);
}
