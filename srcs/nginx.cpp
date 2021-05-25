#include "webserv.hpp"


Nginx::Nginx() : fd_max(-1)
{
	this->fd_pool.resize(1024, NULL);
	FT_FD_ZERO(&(this->reads));
	FT_FD_ZERO(&(this->writes));
	FT_FD_ZERO(&(this->errors));
}

Nginx::~Nginx()
{
	std::cerr << "Nginx Down" << std::endl;
	cleanUp();
}

void	Nginx::cleanUp()
{
	for (std::vector<Fdmanager *>::iterator iter = this->fd_pool.begin(); iter != this->fd_pool.end(); iter++)
	{
		if (*iter != NULL)
		{
			std::cout << "fd : " << (*iter)->getType() << ", type : " << (*iter)->getType() << "  >>  deleted" << std::endl;
			deleteFromFdPool(*iter);
		}
	}
}

void	Nginx::deleteFromFdPool(Fdmanager * fdmanager)
{
	FT_FD_CLR(fdmanager->getFd(), &(this->reads));
	FT_FD_CLR(fdmanager->getFd(), &(this->writes));
	FT_FD_CLR(fdmanager->getFd(), &(this->errors));
	
	if (fdmanager->getType() == FD_RESOURCE)
	{
		Resource *res = dynamic_cast<Resource *>(fdmanager);
		if (res->getClient() != NULL) // 클라이언트가 지시한게 아니다. 클라이언트가 가지고 있는 리스트에서 자기 정보 삭제.
		{
			std::list<Resource *>&resources = res->getClient()->getResponse().getResources();
			for (std::list<Resource *>::iterator iter = resources.begin(); iter!= resources.end(); iter++)
			{
				if (*iter == res)
				{
					resources.erase(iter);
					break ;
				}
			}
		}
		if ((res->getPid() != -1)  && (!res->isReady()) ) // 자식이 돌고있다.
			kill(res->getPid(), SIGINT); // 자식도 죽여준다.
		if (res->getUnlinkPath() != "") // 혹시나 임시파일이있다면 지워준다.
			unlink(res->getUnlinkPath().c_str());
	}

	close(fdmanager->getFd());
	this->fd_pool[fdmanager->getFd()] = NULL;
	if (this->fd_max == fdmanager->getFd())
		this->fd_max--;
	if (fdmanager->getType() != FD_SERVER)
		delete fdmanager;
	return ;
}

void	Nginx::insertToFdpool(Fdmanager *fdmanager) // 이미 new 가 되어 들어온다.
{
	int fd = fdmanager->getFd();

	switch (fdmanager->getType())
	{
	case FD_CLIENT:
	{
		fcntl(fd, F_SETFL, O_NONBLOCK);
		FT_FD_SET(fd, &(this->reads));
		FT_FD_SET(fd, &(this->writes));
		FT_FD_SET(fd, &(this->errors));
		if (this->fd_max < fd)
			this->fd_max = fd;
		this->fd_pool[fd] = fdmanager;
		break ;
	}
	case FD_RESOURCE:
	{
		Resource *res = dynamic_cast<Resource *>(fdmanager);
		if (res->isFdToRawData()) // read해서 어딘가의 client 의 raw 에 적어야한다.
		{
			FT_FD_SET(fd, &(this->reads));
			FT_FD_SET(fd, &(this->errors));
		}
		else if (res->isRawDataToFd()) // 어딘가의 client 의 raw에서 읽어서 fd에 write 해야 한다.
		{
			FT_FD_SET(fd, &(this->writes));
			FT_FD_SET(fd, &(this->errors));
		}
		if (this->fd_max < fd)
			this->fd_max = fd;
		this->fd_pool[fd] = fdmanager;
		break ;
	}
	default:
		break;
	}
}

bool	Nginx::initServers()
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
		if (listen(iter->second.getFd(), CLIENT_QUE_SIZE) == -1)
			throw strerror(errno);

		std::cout << "Server " << iter->second.getServerName() << "(" << iter->second.getIP() << ":" << iter->second.getPort() << ") started" << std::endl;
		// 서버소켓은 read 와 error 만 검사.
		FT_FD_SET(iter->second.getFd(), &(this->reads));
		FT_FD_SET(iter->second.getFd(), &(this->errors));

		this->fd_pool[iter->second.getFd()] = &(iter->second);

		if (this->fd_max < iter->second.getFd())
			this->fd_max = iter->second.getFd();
	}
	return (true);
}

bool	Nginx::run()
{
	fd_set	cpy_reads;
	fd_set	cpy_writes;
	fd_set	cpy_errors;

	struct timeval		select_timeout;
	struct timeval		connection_timeout;

	select_timeout.tv_sec = 5; // last request time out 5000ms
	select_timeout.tv_usec = 0;
	connection_timeout.tv_sec = 20; // last request time out 5000ms
	connection_timeout.tv_usec = 0;

	int		fd_num;

	while (1)
	{
		usleep(5); // cpu 점유가 100% 까지 올라가는 것을 막기 위해서
		cpy_reads = this->reads;
		cpy_writes = this->writes;
		cpy_errors = this->errors;

		if ( (fd_num = select(this->fd_max + 1, &cpy_reads, &cpy_writes, &cpy_errors, &select_timeout)) == -1)
			throw strerror(errno);

		if (fd_num == 0) // select_timeout
			continue ;

		for (int i = 0; i <= this->fd_max; i++)
		{
			if (isIndexOfReadFdSet(i, cpy_reads))
			{
				switch (this->fd_pool[i]->getType())
				{
				case FD_SERVER:
				{
					doReadServerFd(i);
					break;
				}
				case FD_CLIENT:
				{
					doReadClientFD(i);
					break ;
				}
				case FD_RESOURCE:  // read 다 -> 적혀있는 타겟 클라이언트 response 에 적어주면 된다.
				{
					doReadResourceFD(i);
					break ;
				}
				default:
					break;
				}
			}
			else if (isIndexOfWriteFdSet(i, cpy_writes))
			{
				switch (this->fd_pool[i]->getType())
				{
				case FD_CLIENT:
				{
					doWriteClientFD(i);
					break;
				}
				case FD_RESOURCE:
				{
					doWriteResourceFD(i);
					break ;
				}
				default:
					break;
				}
			}
			else if (isIndexOfErrorFdSet(i, cpy_errors))
			{
				std::cout << "error socket : " << i << " deleted" << std::endl;
				deleteFromFdPool(this->fd_pool[i]);
			}
		}
	}
	return (true);
}

//// private
// run()'s
bool	Nginx::isIndexOfReadFdSet(int index, fd_set &reads)
{
	if (FT_FD_ISSET(index, &reads) && (this->fd_pool[index] != NULL))
		return (true);
	else
		return (false);
}

bool	Nginx::isIndexOfWriteFdSet(int index, fd_set &writes)
{
	if (FT_FD_ISSET(index, &writes) && (this->fd_pool[index] != NULL))
		return (true);
	else
		return (false);
}

bool	Nginx::isIndexOfErrorFdSet(int index, fd_set &errors)
{
	if (FT_FD_ISSET(index, &errors) && (this->fd_pool[index] != NULL))
		return (true);
	else
		return (false);
}

void	Nginx::doReadServerFd(int i)
{
	Server *server = dynamic_cast<Server *>(this->fd_pool[i]);
	struct sockaddr_in  client_addr;
	socklen_t			addr_size = sizeof(client_addr);

	int client_socket = accept(i, (struct sockaddr*)&client_addr, &addr_size);

	Client* temp_client = new Client(server, client_socket);
	temp_client->setLastRequestMs(ft_get_time());

	insertToFdpool(temp_client);
}

void	Nginx::doReadClientFD(int i)
{
	Client *client = dynamic_cast<Client *>(this->fd_pool[i]);
	int		len;
	char	buf[BUFFER_SIZE + 1];

	client->setLastRequestMs(ft_get_time());
	len = read(i, buf, BUFFER_SIZE);
	if (len == 0)
		deleteFromFdPool(client);
	else if (len == -1)
		return ;
	else
	{
		buf[len] = 0;
		client->getRequest().getRawRequest() += buf; // 무조건 더한다. (다음 리퀘스트가 미리 와있을 수 있다.)

		//추후에 추가되어야 할 부분입니다. (makeResponse 와 tryMakeRequest 가 대폭 수정 될 예정)
	 	if ((client->getStatus() == REQUEST_RECEIVING) && (client->getRequest().tryMakeRequest() == true))
		{
			client->setStatus(RESPONSE_MAKING);
	 		client->getResponse().makeResponse();
		}
	}
}

void	Nginx::doReadResourceFD(int i)
{
	int len;
	Resource *resource = dynamic_cast<Resource *>(this->fd_pool[i]);
	char	buf[BUFFER_SIZE + 1];

	switch (resource->isReady())
	{
	case READY:
	{
		len = read(resource->getFd(), buf, BUFFER_SIZE);
		if (len == -1)
			return ;
		buf[len] = 0;
		resource->getRawData() += buf;
		if (len < BUFFER_SIZE) // 다읽었다.
		{
			resource->doNext();
			if (resource->getPid() != -1)
				unlink(resource->getUnlinkPath().c_str());
			deleteFromFdPool(resource);
		}
		break ;
	}
	case NOT_YET: // 아직 준비가 되지 않았다.
	{
		break ;
	}
	case CGI_CRASH:
	{
		resource->getClient()->getResponse().makeErrorResponse(500); // 에러 리스폰스 만들러감
		deleteFromFdPool(resource);
		break ;
	}
	default:
		break ;
	}
}

void	Nginx::doWriteClientFD(int i)
{
	Client *client = dynamic_cast<Client *>(this->fd_pool[i]);

	if (client->getStatus() == RESPONSE_MAKE_DONE)
	{
		int len;
		size_t w_idx = client->getResponse().getWriteIndex();
		const char *current_str = client->getResponse().getRawResponse().c_str() + w_idx;

		len = write(i, current_str, client->getResponse().getRawResponse().size() - w_idx);
		if (len < 0)
			return ;
		if ((size_t) len < client->getResponse().getRawResponse().size() - w_idx) // 다 안쓰였다.
			client->getResponse().setWriteIndex( w_idx + len );
		else // 다쓰였다.
		{
			if (client->getResponse().getIsDisconnectImmediately())
				deleteFromFdPool(client);
			else
			{
				client->getRequest().initRequest();
				client->getResponse().initResponse();
				client->setStatus(REQUEST_RECEIVING);
			}
		}
	}
}

void    Nginx::doWriteResourceFD(int i)
{
    Resource *res = dynamic_cast<Resource *>(this->fd_pool[i]);
	int len;

	size_t w_idx = res->getWriteIndex();
	const char *current_str = (res->getRawData().c_str() + w_idx);

	len = write(res->getFd(), current_str, (res->getRawData().size() - w_idx));
	if (len < 0)
		return ;
	if ((size_t)len < (res->getRawData().size() - w_idx))
		res->setWriteIndex( w_idx + len );
	else
	{
        res->doNext();
        deleteFromFdPool(res);
	}
}
