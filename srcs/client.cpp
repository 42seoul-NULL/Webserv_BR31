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
}

Client::~Client()
{

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
