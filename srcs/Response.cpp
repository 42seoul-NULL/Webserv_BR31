#include "response.hpp"
#include "config.hpp"
#include "nginx.hpp"

Response::Response(void)
{
	initResponse();
}
/////// geter ////////
int		Response::getLastResponse()
{
	return (this->last_reponse);
}

std::string&	Response::getRawResponse(void)
{
	return (this->raw_response);
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

