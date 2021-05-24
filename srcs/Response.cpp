#include "webserv.hpp"

Response::Response(void)
{
	initResponse();
}

Response::~Response()
{
//	std::cout << "response destroyer called" << std::endl;

	for (std::list<Resource *>::iterator iter = this->resources.begin(); iter != this->resources.end(); iter++)
		Config::getInstance()->getNginx()->deleteFromFdPool(*iter);
}

/////// geter ////////
bool		Response::getIsDisconnectImmediately()
{
	return (this->is_disconnect_immediately);
}

std::string&	Response::getRawResponse(void)
{
	return (this->raw_response);
}

size_t			Response::getWriteIndex()
{
	return (this->write_index);
}

/////// setter ///////
void	Response::setLocation(Location *location)
{
	this->location = location;
}

void	Response::setClient(Client *client)
{
	this->client = client;
}

void	Response::setResourcePath(const std::string &resource_path)
{
	this->resource_path = resource_path;
}

void	Response::setCgiExtention(const std::string &cgi_extention)
{
	this->cgi_extention = cgi_extention;
}

void	Response::setIsRedirection(bool is_redirection)
{
	this->is_redirection = is_redirection;
}

void	Response::setWriteIndex(size_t index)
{
	this->write_index = index;
}
