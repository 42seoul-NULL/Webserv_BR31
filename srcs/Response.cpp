#include "webserv.hpp"

Response::Response(void)
{
	this->raw_response.clear();
	this->resource_path.clear();
	this->is_redirection = false;
	this->cgi_extention.clear();
	this->location = NULL;
	this->write_index = 0;
	this->resources.clear();
	this->is_disconnect_immediately = false;
}

Response::~Response()
{
	for (std::list<Resource *>::iterator iter = this->resources.begin(); iter != this->resources.end(); iter++)
	{
		(*iter)->setClient(NULL);
		Config::getInstance()->getNginx()->deleteFromFdPool(*iter);
	}
}

/////// geter ////////
std::list<Resource *> &Response::getResources()
{
	return (this->resources);
}

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
