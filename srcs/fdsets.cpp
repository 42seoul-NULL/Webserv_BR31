#include "webserv.hpp"

Fdsets::Fdsets()
{
	FT_FD_ZERO(&(this->read_fd_set));
	FT_FD_ZERO(&(this->write_fd_set));
	FT_FD_ZERO(&(this->error_fd_set));
}

Fdsets::~Fdsets()
{}

Fdsets		&Fdsets::operator=(Fdsets &rvalue)
{
	if (this != &rvalue)
	{
		this->read_fd_set = rvalue.getErrorFdSet();
		this->write_fd_set = rvalue.getWriteFdSet();
		this->error_fd_set = rvalue.getErrorFdSet();
	}
	return (*this);
}

fd_set&	Fdsets::getReadFdSet()
{
	return (this->read_fd_set);
}

fd_set&	Fdsets::getWriteFdSet()
{
	return (this->write_fd_set);
}

fd_set&	Fdsets::getErrorFdSet()
{
	return (this->error_fd_set);

}

void			Fdsets::addToReadSet(int fd)
{
	FT_FD_SET(fd, &(this->read_fd_set));
}

void			Fdsets::addToWriteSet(int fd)
{
	FT_FD_SET(fd, &(this->write_fd_set));
}

void			Fdsets::addToErrorSet(int fd)
{
	FT_FD_SET(fd, &(this->error_fd_set));
}

void			Fdsets::deleteFd(int fd)
{
	FT_FD_CLR(fd, &(this->read_fd_set));
	FT_FD_CLR(fd, &(this->write_fd_set));
	FT_FD_CLR(fd, &(this->error_fd_set));
}

bool	Fdsets::fdIsInReadFdSet(int fd)
{
	if (FT_FD_ISSET(fd, &this->read_fd_set))
		return (true);
	else
		return (false);
}

bool	Fdsets::fdIsInWriteFdSet(int fd)
{
	if (FT_FD_ISSET(fd, &this->write_fd_set))
		return (true);
	else
		return (false);
}

bool	Fdsets::fdIsInErrorFdSet(int fd)
{
	if (FT_FD_ISSET(fd, &this->error_fd_set))
		return (true);
	else
		return (false);
}
