#include "webserv.hpp"

Client::Client()
{
	this->server = NULL;
	this->fd = -1;
	this->status = REQUEST_RECEIVING;
	this->type = FD_CLIENT;
}

Client::Client(Server *server, int fd)
{
	this->fd = fd;
	this->status = REQUEST_RECEIVING;
	this->type = FD_CLIENT;
	this->server = server;
	this->request.setClient(this);
	this->response.setClient(this);
	this->fd_read = -1;
	this->fd_write = -1;
}

Client::~Client()
{
	if (fd_read != -1)
		close(fd_read);
	if (fd_write != -1)
		close(fd_write);
}
//setter
void	Client::setLastRequestMs(unsigned long last_request_ms)
{
	this->last_request_ms = last_request_ms;
}

void	Client::setStatus(e_client_status status)
{
	this->status = status;
	return ;
}

void		Client::setFdRead(int fd_read)
{
	this->fd_read = fd_read;
}

void		Client::setFdWrite(int fd_write)
{
	this->fd_write = fd_write;
}

//getter
e_client_status	Client::getStatus()
{
	return (this->status);
}

Server *	Client::getServer()
{
	return (this->server);
}

Request		&Client::getRequest()
{
	return (this->request);
}

Response	&Client::getResponse()
{
	return (this->response);
}

unsigned long	Client::getLastRequestMs()
{
	return (this->last_request_ms);
}

int		Client::getFdRead()
{
	return (this->fd_read);
}

int		Client::getFdWrite()
{
	return (this->fd_write);
}

//others
void	Client::appendRawRequest(const char *buf)
{
	this->request.getRawRequest() += buf;
}

bool	Client::tryMakeRequest()
{
	return (this->request.tryMakeRequest());
}

void	Client::makeResponse()
{
	this->response.makeResponse();
}
