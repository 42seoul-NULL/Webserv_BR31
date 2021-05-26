#include "webserv.hpp"


Nginx::Nginx()
{
	this->fd_pool.resize(1024, NULL);
	// FT_FD_ZERO(&(this->fd_sets.read_fd_set));
	// FT_FD_ZERO(&(this->writes.write_fd_set));
	// FT_FD_ZERO(&(this->errors));
}

Nginx::~Nginx()
{
	std::cerr << "Nginx Down" << std::endl;
	cleanUp();
}

bool	Nginx::initServers()
{
	for (serverMap::iterator server_iter = Config::getInstance()->getServerBlocks().begin(); server_iter != Config::getInstance()->getServerBlocks().end(); server_iter++)
	{
		turnOnServerFD(server_iter);
		putServerFdIntoFdPool(server_iter);
	}
	return (true);
}

bool	Nginx::run()
{
	int		ready_fd_num;

	while (1)
	{
		Fdsets copy_fd_sets = this->fd_sets;

		ready_fd_num = selectReadyFd(copy_fd_sets);
		if (ready_fd_num == 0) // select_timeout
			continue ;
		workWithReadyFds(copy_fd_sets);
		usleep(5);
	}
	return (true);
}

void	Nginx::deleteFromFdPool(Fdmanager * fdmanager)
{
	this->fd_sets.deleteFd(fdmanager->getFd());
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

	if (fdmanager->getType() == FD_RESOURCE)
		std::cout << "Resource >> close fd : " << fdmanager->getFd() << std::endl;
	else if (fdmanager->getType() == FD_CLIENT)
		std::cout << "Client >> close fd : " << fdmanager->getFd() << std::endl;

	this->fd_pool[fdmanager->getFd()] = NULL;
	if (this->fd_max == fdmanager->getFd())
		--this->fd_max;
	if (fdmanager->getType() != FD_SERVER)
		delete fdmanager;
	return ;
}

void	Nginx::insertToFdpool(Fdmanager *fdmanager) // 이미 new 가 되어 들어온다.
{
	int fd = fdmanager->getFd();

	if (this->fd_pool[fd] != NULL)
	{
		std::cout << "try to input same fd" << std::endl;
	}

	switch (fdmanager->getType())
	{
	case FD_CLIENT:
	{
		std::cout << "Client >> socket connection fd : " << fd << std::endl;

		fcntl(fd, F_SETFL, O_NONBLOCK);
		this->fd_sets.addToReadSet(fd);
		this->fd_sets.addToWriteSet(fd);
		this->fd_sets.addToErrorSet(fd);
		if (this->fd_max < fd)
			this->fd_max = fd;
		this->fd_pool[fd] = fdmanager;
		break ;
	}
	case FD_RESOURCE:
	{
		fcntl(fd, F_SETFL, O_NONBLOCK);
		Resource *res = dynamic_cast<Resource *>(fdmanager);
		if (res->isFdToRawData()) // read해서 어딘가의 client 의 raw 에 적어야한다.
		{
			this->fd_sets.addToReadSet(fd);
			this->fd_sets.addToErrorSet(fd);
		}
		else if (res->isRawDataToFd()) // 어딘가의 client 의 raw에서 읽어서 fd에 write 해야 한다.
		{
			this->fd_sets.addToWriteSet(fd);
			this->fd_sets.addToErrorSet(fd);
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

void	Nginx::cleanUp()
{
	for (std::vector<Fdmanager *>::iterator iter = this->fd_pool.begin(); iter != this->fd_pool.end(); iter++)
	{
		if (*iter != NULL)
		{
			std::cout << "fd : " << (*iter)->getFd() << ", type : " << (*iter)->getType() << "  >>  deleted" << std::endl;
			deleteFromFdPool(*iter);
		}
	}
}

void	Nginx::setConnectionTimeOut(size_t connection_time_out)
{
	this->connection_time_out = connection_time_out;
}

//// private
// initServer()'s
void	Nginx::turnOnServerFD(serverMap::iterator server_block)
{
	struct sockaddr_in  server_addr;
		server_block->second.setFd(socket(PF_INET, SOCK_STREAM, 0));
		int option = 1;
		setsockopt(server_block->second.getFd(), SOL_SOCKET, SO_REUSEADDR, &option, sizeof(int));

		ft_memset(&server_addr, 0, sizeof(server_addr));
		server_addr.sin_family = AF_INET;
		server_addr.sin_addr.s_addr = inet_addr(server_block->second.getIP().c_str());
		server_addr.sin_port = ft_htons(server_block->second.getPort());

		if (bind(server_block->second.getFd(), (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
		{
			std::cerr << "bind() error" << std::endl;
			throw strerror(errno);
		}
		if (listen(server_block->second.getFd(), CLIENT_QUE_SIZE) == -1)
			throw strerror(errno);
		std::cout << "Server " << server_block->second.getServerName() << "(" << server_block->second.getIP() << ":" << server_block->second.getPort() << ") started" << std::endl;
}

void	Nginx::putServerFdIntoFdPool(serverMap::iterator server_block)
{
	this->fd_sets.addToReadSet(server_block->second.getFd());
	this->fd_sets.addToErrorSet(server_block->second.getFd());
	// FT_FD_SET(server_block->second.getFd(), &(this->reads));
	// FT_FD_SET(server_block->second.getFd(), &(this->errors));

	this->fd_pool[server_block->second.getFd()] = &(server_block->second);

	if (this->fd_max < server_block->second.getFd())
		this->fd_max = server_block->second.getFd();
}

// run()'s

int				Nginx::selectReadyFd(Fdsets &fd_sets)
{
	struct timeval		select_timeout;

	select_timeout.tv_sec = 5; // last request time out 5000ms
	select_timeout.tv_usec = 0;

	int ready_fd_num;
	ready_fd_num = select(this->fd_max + 1, &fd_sets.getReadFdSet(), &fd_sets.getWriteFdSet(), &fd_sets.getErrorFdSet(), &select_timeout);
	if (ready_fd_num == -1)
	{
		std::cout << "select return -1" << std::endl;
			throw strerror(errno);
	}
	return (ready_fd_num);
}

void	Nginx::workWithReadyFds(Fdsets &fd_sets)
{
	for (int fd = 0; fd <= this->fd_max; fd++)
	{
		if (isReadyReadFd(fd, fd_sets))
			doReadAlongFdType(fd);
		else if (isReadyWriteFd(fd, fd_sets))
			doWriteAlongFdType(fd);
		else if (isReadyErrorFd(fd, fd_sets))
		{
			std::cout << "error socket : " << fd << " deleted" << std::endl;
			deleteFromFdPool(this->fd_pool[fd]);
		}
	}
}

bool	Nginx::isReadyReadFd(int fd, Fdsets &fd_sets)
{
	if (fd_sets.fdIsInReadFdSet(fd) && (this->fd_pool[fd] != NULL))
		return (true);
	else
		return (false);
}

bool	Nginx::isReadyWriteFd(int fd, Fdsets &fd_sets)
{
	if (fd_sets.fdIsInWriteFdSet(fd) && (this->fd_pool[fd] != NULL))
		return (true);
	else
		return (false);
}

bool	Nginx::isReadyErrorFd(int fd, Fdsets &fd_sets)
{
	if (fd_sets.fdIsInErrorFdSet(fd) && (this->fd_pool[fd] != NULL))
		return (true);
	else
		return (false);
}

void	Nginx::doReadAlongFdType(int fd)
{
	switch (this->fd_pool[fd]->getType())
	{
	case FD_SERVER:
	{
		doReadServerFd(fd);
		break;
	}
	case FD_CLIENT:
	{
		doReadClientFD(fd);
		break ;
	}
	case FD_RESOURCE:  // read 다 -> 적혀있는 타겟 클라이언트 response 에 적어주면 된다.
	{
		doReadResourceFD(fd);
		break ;
	}
	default:
		break;
	}
}

void	Nginx::doReadServerFd(int fd)
{
	Server *server = dynamic_cast<Server *>(this->fd_pool[fd]);
	struct sockaddr_in  client_addr;
	socklen_t			addr_size = sizeof(client_addr);

	int client_socket = accept(fd, (struct sockaddr*)&client_addr, &addr_size);

	Client* temp_client = new Client(server, client_socket);
	temp_client->setLastRequestMs(ft_get_time());

	insertToFdpool(temp_client);
}

void	Nginx::doReadClientFD(int fd)
{
	Client *client = dynamic_cast<Client *>(this->fd_pool[fd]);
	int		len;
	char	buf[BUFFER_SIZE + 1];

	client->setLastRequestMs(ft_get_time());
	len = read(fd, buf, BUFFER_SIZE);
	if (len <= 0)
	{
		if (len == 0)
		{
			std::cout << "Client : " << fd << " read 0 || ";
			deleteFromFdPool(client);
		}
		else
			std::cout << "Client : " << fd << " read minus" << std::endl;
	}
	else
	{
		buf[len] = 0;
		client->getRequest().getRawRequest() += buf; // 무조건 더한다. (다음 리퀘스트가 미리 와있을 수 있다.)
	 	if ((client->getStatus() == REQUEST_RECEIVING) && (client->getRequest().tryMakeRequest() == true))
		{
			client->setStatus(RESPONSE_MAKING);
	 		client->getResponse().makeResponse();
		}
	}
}

void	Nginx::doReadResourceFD(int fd)
{
	int len;
	Resource *resource = dynamic_cast<Resource *>(this->fd_pool[fd]);
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

void	Nginx::doWriteAlongFdType(int fd)
{
	switch (this->fd_pool[fd]->getType())
	{
	case FD_CLIENT:
	{
		doWriteClientFD(fd);
		break;
	}
	case FD_RESOURCE:
	{
		doWriteResourceFD(fd);
		break ;
	}
	default:
		break;
	}
}


void	Nginx::doWriteClientFD(int i)
{
	Client *client = dynamic_cast<Client *>(this->fd_pool[i]);

	if ((client->getStatus() == REQUEST_RECEIVING) && ((ft_get_time() - client->getLastRequestMs()) > this->connection_time_out ))
	{
		deleteFromFdPool(client);
		return ;
	}
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
