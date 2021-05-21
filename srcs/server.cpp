#include "webserv.hpp"

/////////////////////////////////////////////////////////
////////////////// class Server start ///////////////////
Server::Server() : port(-1)
{
	this->type = FD_SERVER;
}

Server::Server(const Server& src)
{
	this->type = FD_SERVER;
	this->ip = src.ip;
	this->port	=	src.port;
	this->server_name	=	src.server_name;
	this->fd		=	src.fd;
	this->locations.insert(src.locations.begin(), src.locations.end());
}

Server &Server::operator=(const Server &src)
{
	this->type = FD_SERVER;
	this->ip = src.ip;
	this->port	=	src.port;
	this->server_name	=	src.server_name;
	this->fd		=	src.fd;
	this->locations.clear();
	this->locations.insert(src.locations.begin(), src.locations.end());

	return (*this);
}

Server::~Server()
{
	return ;
}

Location	&Server::getPerfectLocation(std::string &uri)
{
	Location *ret = &(this->locations["/"]);
	std::string key = "";
	for (std::string::const_iterator iter = uri.begin(); iter != uri.end(); iter++)
	{
		key += *iter;
		if (*iter == '/')
		{
			if (this->locations.find(key) == this->locations.end())
				return (*ret);
			else
				ret = &(this->locations[key]);
		}
	}
	if ( *(--key.end()) != '/') // '/'로 끝나지 않았고
	{
		key += '/';
		if (this->locations.find(key) != this->locations.end())
		{
			uri = key;
			return (this->locations[key]);
		}
	}
	return (*ret);
}

void	Server::setPort(unsigned short port)
{
	this->port = port;
	return ;
}

void	Server::setIP(const std::string &ip)
{
	this->ip = ip;
	return ;
}

void	Server::setServerName(const std::string &server_name)
{
	this->server_name = server_name;
	return ;
}

const std::string &Server::getIP() const
{
	return (this->ip);
}

const std::string &Server::getServerName() const
{
	return (this->server_name);
}

unsigned short		Server::getPort() const
{
	return (this->port);
}

std::map<std::string, Location> &Server::getLocations()
{
	return (this->locations);
}

//for test
void		Server::show()
{
	std::cout << "ip	:	" << this->ip << std::endl;
	std::cout << "port	:	" << this->port << std::endl;
	std::cout << "server_name	:	" << this->server_name << std::endl;
	std::cout << "============= location start =============" << std::endl;
	for (std::map<std::string, Location>::iterator iter = locations.begin(); iter != locations.end(); iter++)
	{
		std::cout << "=== Location Key : " << iter->first << " ===" << std::endl;
		iter->second.show();
	}
	std::cout << "============= location end ===============" << std::endl;
}

////////////////// class Server end /////////////////////
/////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////
///////////////// class Location start //////////////////
Location::Location() : request_max_body_size(-1), redirect_return(-1)
{
	
}

Location::Location(const Location &src)
{
	this->root	=	src.root;
	this->index.assign(src.index.begin(), src.index.end());
	this->allow_methods.assign(src.allow_methods.begin(), src.allow_methods.end());
	this->request_max_body_size = src.request_max_body_size;
	this->error_pages.insert(src.error_pages.begin(), src.error_pages.end());
	this->upload_path = src.upload_path;
	this->auto_index = src.auto_index;
	this->cgi_infos.insert(src.cgi_infos.begin(), src.cgi_infos.end());
	this->auth_key = src.auth_key;
	this->redirect_addr = src.redirect_addr;
	this->redirect_return = src.redirect_return;
	this->uri_key = src.uri_key;
}

Location &Location::operator=(const Location &src)
{
	this->root	=	src.root;
	this->index.assign(src.index.begin(), src.index.end());
	this->allow_methods.assign(src.allow_methods.begin(), src.allow_methods.end());
	this->request_max_body_size = src.request_max_body_size;
	this->error_pages.insert(src.error_pages.begin(), src.error_pages.end());
	this->upload_path = src.upload_path;
	this->auto_index = src.auto_index;
	this->cgi_infos.insert(src.cgi_infos.begin(), src.cgi_infos.end());
	this->auth_key = src.auth_key;
	this->redirect_addr = src.redirect_addr;
	this->redirect_return = src.redirect_return;
	this->uri_key = src.uri_key;
	return (*this);
}

void		Location::setRoot(const std::string& root)
{
	this->root = root;
	return ;
}

void		Location::setRequestMaxBodySize(int request_max_body_size)
{
	this->request_max_body_size = request_max_body_size;
	return ;
}
void		Location::setUploadPath(const std::string &upload_path)
{
	this->upload_path = upload_path;
	return ;
}

void		Location::setAutoIndex(bool auto_index)
{
	this->auto_index = auto_index;
	return ;
}

void		Location::setAuthKey(const std::string &auth_key)
{
	this->auth_key = auth_key;
	return ;
}

void		Location::setRedirectReturn(int redirect_return)
{
	this->redirect_return = redirect_return;
	return ;
}

void		Location::setRedirectAddr(const std::string &redirect_addr)
{
	this->redirect_addr = redirect_addr;
	return ;
}

void		Location::setUriKey(const std::string &uri_key)
{
	this->uri_key = uri_key;
	return ;
}

const std::string &Location::getUriKey()
{
	return (this->uri_key);
}

const std::string &Location::getRoot()
{
	return (this->root);
}

std::list<std::string> &Location::getIndex()
{
	return (this->index);
}

std::list<std::string> &Location::getAllowMethods()
{
	return (this->allow_methods);
}

int Location::getRequestMaxBodySize()
{
	return (this->request_max_body_size);
}

std::map<int, std::string> &Location::getErrorPages()
{
	return (this->error_pages);
}

const std::string &Location::getUploadPath()
{
	return (this->upload_path);
}

bool	Location::getAutoIndex()
{
	return (this->auto_index);
}

std::map<std::string, std::string>& Location::getCgiInfos()
{
	return (this->cgi_infos);
}

const std::string &Location::getAuthKey()
{
	return (this->auth_key);
}

int		Location::getRedirectReturn()
{
	return (this->redirect_return);
}

const std::string &Location::getRedirectAddr()
{
	return (this->redirect_addr);
}

//for test
void	Location::show()
{
	std::cout << "root	:	" << this->root << std::endl;
	std::cout << "rqmbs	:	" << this->request_max_body_size << std::endl;
	std::cout << "upload_path	:	" << this->upload_path << std::endl;
	std::cout << "auto_index	:	" << this->auto_index << std::endl;
	std::cout << "auth_key	:	" << this->auth_key << std::endl;
	std::cout << "index	: ";
	for (std::list<std::string>::iterator iter = this->index.begin(); iter != this->index.end(); iter++)
		std::cout << *iter << " ";
	std::cout << std::endl;
	std::cout << "allow_methods	: ";
	for (std::list<std::string>::iterator iter = this->allow_methods.begin(); iter != this->allow_methods.end(); iter++)
		std::cout << *iter << " ";
	std::cout << std::endl;
	std::cout << "error_pages	: " << std::endl;
	for (std::map<int, std::string>::iterator iter = this->error_pages.begin(); iter != this->error_pages.end(); iter++)
		std::cout << iter->first << " | " << iter->second << std::endl;
	std::cout << "cgi_infos	: " << std::endl;
	for (std::map<std::string, std::string>::iterator iter = this->cgi_infos.begin(); iter != this->cgi_infos.end(); iter++)
		std::cout << iter->first << " | " << iter->second << std::endl;
}

///////////////// class Location end ////////////////////
/////////////////////////////////////////////////////////