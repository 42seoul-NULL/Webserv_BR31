#include "webserv.hpp"


Fdmanager::Fdmanager() : fd(-1), type(FD_BASE)
{

}

Fdmanager::~Fdmanager()
{

}

void Fdmanager::setFd(int fd)
{
	this->fd = fd;
	return ;
}

void	Fdmanager::setType(e_fd_type type)
{
	this->type = type;
	return ;
}

e_fd_type	Fdmanager::getType() const
{
	return (this->type);
}

int		Fdmanager::getFd() const
{
	return (this->fd);
}