#include "../includes/parser.hpp"

/////////////////////////////////////////////////////////
/////////////////// class Config start //////////////////
Config* Config::instance;

int const Config::decodeMimeBase64[256] = {
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 00-0F */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 10-1F */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,62,-1,-1,-1,63,  /* 20-2F */
    52,53,54,55,56,57,58,59,60,61,-1,-1,-1,-1,-1,-1,  /* 30-3F */
    -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,  /* 40-4F */
    15,16,17,18,19,20,21,22,23,24,25,-1,-1,-1,-1,-1,  /* 50-5F */
    -1,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,  /* 60-6F */
    41,42,43,44,45,46,47,48,49,50,51,-1,-1,-1,-1,-1,  /* 70-7F */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 80-8F */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 90-9F */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* A0-AF */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* B0-BF */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* C0-CF */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* D0-DF */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* E0-EF */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1   /* F0-FF */
};

Config::Config()
{
	this->mime_type[".aac"] = "audio/aac";
	this->mime_type[".abw"] = "application/x-abiword";
	this->mime_type[".arc"] = "application/octet-stream";
	this->mime_type[".avi"] = "video/x-msvideo";
	this->mime_type[".azw"] = "application/vnd.amazon.ebook";
	this->mime_type[".bin"] = "application/octet-stream";
	this->mime_type[".bz"] = "application/x-bzip";
	this->mime_type[".bz2"] = "application/x-bzip2";
	this->mime_type[".csh"] = "application/x-csh";
	this->mime_type[".css"] = "text/css";
	this->mime_type[".csv"] = "text/csv";
	this->mime_type[".doc"] = "application/msword";
	this->mime_type[".epub"] = "application/epub+zip";
	this->mime_type[".gif"] = "image/gif";
	this->mime_type[".htm"] = "text/html";
	this->mime_type[".html"] = "text/html";
	this->mime_type[".ico"] = "image/x-icon";
	this->mime_type[".ics"] = "text/calendar";
	this->mime_type[".jar"] = "Temporary Redirect";
	this->mime_type[".jpeg"] = "image/jpeg";
	this->mime_type[".jpg"] = "image/jpeg";
	this->mime_type[".js"] = "application/js";
	this->mime_type[".json"] = "application/json";
	this->mime_type[".mid"] = "audio/midi";
	this->mime_type[".midi"] = "audio/midi";
	this->mime_type[".mpeg"] = "video/mpeg";
	this->mime_type[".mpkg"] = "application/vnd.apple.installer+xml";
	this->mime_type[".odp"] = "application/vnd.oasis.opendocument.presentation";
	this->mime_type[".ods"] = "application/vnd.oasis.opendocument.spreadsheet";
	this->mime_type[".odt"] = "application/vnd.oasis.opendocument.text";
	this->mime_type[".oga"] = "audio/ogg";
	this->mime_type[".ogv"] = "video/ogg";
	this->mime_type[".ogx"] = "application/ogg";
	this->mime_type[".pdf"] = "application/pdf";
	this->mime_type[".ppt"] = "application/vnd.ms-powerpoint";
	this->mime_type[".rar"] = "application/x-rar-compressed";
	this->mime_type[".rtf"] = "application/rtf";
	this->mime_type[".sh"] = "application/x-sh";
	this->mime_type[".svg"] = "image/svg+xml";
	this->mime_type[".swf"] = "application/x-shockwave-flash";
	this->mime_type[".tar"] = "application/x-tar";
	this->mime_type[".tif"] = "image/tiff";
	this->mime_type[".tiff"] = "image/tiff";
	this->mime_type[".ttf"] = "application/x-font-ttf";
	this->mime_type[".vsd"] = " application/vnd.visio";
	this->mime_type[".wav"] = "audio/x-wav";
	this->mime_type[".weba"] = "audio/webm";
	this->mime_type[".webm"] = "video/webm";
	this->mime_type[".webp"] = "image/webp";
	this->mime_type[".woff"] = "application/x-font-woff";
	this->mime_type[".xhtml"] = "application/xhtml+xml";
	this->mime_type[".xls"] = "application/vnd.ms-excel";
	this->mime_type[".xml"] = "application/xml";
	this->mime_type[".xul"] = "application/vnd.mozilla.xul+xml";
	this->mime_type[".zip"] = "application/zip";
	this->mime_type[".3gp"] = "video/3gpp audio/3gpp";
	this->mime_type[".3g2"] = "video/3gpp2 audio/3gpp2";
	this->mime_type[".7z"] = "application/x-7z-compressed";

	this->status_code["100"] = "Continue";
	this->status_code["101"] = "Switching Protocols";
	this->status_code["102"] = "Processing";
	this->status_code["200"] = "OK";
	this->status_code["201"] = "Created";
	this->status_code["202"] = "Accepted";
	this->status_code["203"] = "Non-authoritative Information";
	this->status_code["204"] = "No Content";
	this->status_code["205"] = "Reset Content";
	this->status_code["206"] = "Partial Content";
	this->status_code["207"] = "Multi-Status";
	this->status_code["208"] = "Already Reported";
	this->status_code["226"] = "IM Used";
	this->status_code["300"] = "Multiple Choices";
	this->status_code["301"] = "Moved Permanently";
	this->status_code["302"] = "Found";
	this->status_code["303"] = "See Other";
	this->status_code["304"] = "Not Modified";
	this->status_code["305"] = "Use Proxy";
	this->status_code["307"] = "Temporary Redirect";
	this->status_code["308"] = "Permanent Redirect";
	this->status_code["400"] = "Bad Request";
	this->status_code["401"] = "Unauthorized";
	this->status_code["402"] = "Payment Required";
	this->status_code["403"] = "Forbidden";
	this->status_code["404"] = "Not found";
	this->status_code["405"] = "Method Not Allowed";
	this->status_code["406"] = "Not Acceptable";
	this->status_code["407"] = "Proxy Authentication Required";
	this->status_code["408"] = "Required Timeout";
	this->status_code["409"] = "Conflict";
	this->status_code["410"] = "Gone";
	this->status_code["411"] = "Length Required";
	this->status_code["412"] = "Precondition Failed";
	this->status_code["413"] = "Request Entity Too Large";
	this->status_code["414"] = "Request URI Too Long";
	this->status_code["415"] = "Unsupported Media Type";
	this->status_code["416"] = "Requested Range Not Satisfiable";
	this->status_code["417"] = "Expectation Failed";
	this->status_code["418"] = "IM_A_TEAPOT";
	this->status_code["500"] = "Internal Server Error";
	this->status_code["501"] = "Not Implemented";
	this->status_code["502"] = "Bad Gateway";
	this->status_code["503"] = "Service Unavailable";
	this->status_code["504"] = "Gateway Timeout";
	this->status_code["505"] = "HTTP Version Not Supported";
	this->status_code["506"] = "Variant Also Negotiates";
	this->status_code["507"] = "Insufficient Storage";
	this->status_code["508"] = "Loop Detected";
	this->status_code["510"] = "Not Extened";
	this->status_code["511"] = "Network Authentication Required";
	this->status_code["599"] = "Network Connect Timeout Error";
}

Config::Config(const Config &src)
{
	(void)src;
}

Config &Config::operator=(const Config &src)
{
	(void)src;
	return (*this);
}

Config::~Config()
{
	delete (this->instance);
	return ;
}

Config *Config::getInstance()
{
	if (instance == NULL)
		instance = new Config();
	return (instance);
}

bool	Config::returnFalseWithMsg(const char *str)
{
	std::cerr << str << std::endl;
	return (false);
}

std::map<std::string, Server> &Config::getServers()
{
	return (this->servers);
}

std::map<std::string, std::string> &Config::getMimeType()
{
	return (this->mime_type);
}

std::map<std::string, std::string> &Config::getStatusCode()
{
	return (this->status_code);
}

bool	Config::isReserved(const std::string &src)
{
	if (src == "server" || 
		src == "listen" || 
		src == "server_name" || 
		src == "location" || 
		src == "error_page" || 
		src == "allow_methods" || 
		src == "root" ||
		src == "index" ||
		src == "upload_path" ||
		src == "auto_index" ||
		src == "request_max_body_size" ||
		src == "auth_key" ||
		src == "cgi_extension" ||
		src == "return" ||
		src == "}" ||
		src == "{" )
		return (true);
	return (false);
}

bool	Config::makeConfig(const char *path)
{
	int				fd;
	std::string		line;
	int				ret;
	std::string		splited;
	std::vector<std::string> vec;
	std::string		key;
	std::string		location_name;

	fd = open(path, O_RDONLY);
	if (fd < 3)
		return (returnFalseWithMsg("Can't open config"));

	try
	{
		while ( (ret = get_next_line(fd, line)) > 0)
		{
			if (line == "")
				continue ;
			splited = ft_split(line, " \t", vec);
			line.clear();
		}

		for (std::vector<std::string>::iterator iter = vec.begin(); iter != vec.end(); iter++)
		{
			if (*iter == "server_name")
			{
				iter++;
				std::string server_name = *iter;
				iter++; // listen
				iter++; // 8080
				std::string port = *iter;
				iter++; // 127.0.0.1
				key = server_name + ":" + port;
				if (instance->servers.find(key) != instance->servers.end()) // 이미 존재
					throw "server_name and port already exists";
				instance->servers[key].setServerName(server_name);
				instance->servers[key].setPort(ft_atoi(port));
				instance->servers[key].setIP(*iter);
			}
			else if (*iter == "location")
			{
				iter++;
				location_name = *iter;
			}
			else if (*iter == "error_page")
			{
				iter++;
				int key2 = ft_atoi(*iter);
				iter++;
				instance->servers[key].getLocations()[location_name].getErrorPages()[key2] = *iter;
			}
			else if (*iter == "allow_methods")
			{
				iter++;
				while (!isReserved(*iter) && iter != vec.end())
				{
					instance->servers[key].getLocations()[location_name].getAllowMethods().push_back(*iter);
					iter++;
				}
				if (iter == vec.end())
					break ;
				iter--;
			}
			else if (*iter == "root")
			{
				iter++;
				instance->servers[key].getLocations()[location_name].setRoot(*iter);
			}
			else if (*iter == "index")
			{
				iter++;
				while (!isReserved(*iter) && iter != vec.end())
				{
					instance->servers[key].getLocations()[location_name].getIndex().push_back(*iter);
					iter++;
				}
				if (iter == vec.end())
					break ;
				iter--;
			}
			else if (*iter == "upload_path")
			{
				iter++;
				instance->servers[key].getLocations()[location_name].setUploadPath(*iter);
			}
			else if (*iter == "auto_index")
			{
				iter++;
				if (*iter == "on")
					instance->servers[key].getLocations()[location_name].setAutoIndex(true);
				else
					instance->servers[key].getLocations()[location_name].setAutoIndex(false);
			}
			else if (*iter == "request_max_body_size")
			{
				iter++;
				instance->servers[key].getLocations()[location_name].setRequestMaxBodySize(ft_atoi(*iter));
			}
			else if (*iter == "cgi_extension")
			{
				iter++;
				instance->servers[key].getLocations()[location_name].setCgiExtension(*iter);
			}
			else if (*iter == "auth_key")
			{
				iter++;
				instance->servers[key].getLocations()[location_name].setAuthKey(*iter);
			}
			else if (*iter == "return")
			{
				iter++;
				instance->servers[key].getLocations()[location_name].setRedirectReturn(ft_atoi(*iter));
				iter++;
				instance->servers[key].getLocations()[location_name].setRedirectAddr(*iter);				
			}
		}
	}
	catch(const char *e)
	{
		std::cout << e << std::endl;
		return (false);
	}
	return (true);	
}

//for test
void		Config::show()
{
	for (std::map<std::string, Server>::iterator iter = this->servers.begin(); iter != this->servers.end(); iter++)
	{
		std::cout << "server key : " << iter->first << std::endl;
		iter->second.show();
	}
}
/////////////////// class Config end ////////////////////
/////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////
////////////////// class Client start ///////////////////
Client::Client()
{
	this->server_socket_fd = -1;
	this->socket_fd = -1;
	this->status = REQUEST_RECEIVING;
}

Client::Client(int server_socket_fd, int socket_fd) : server_socket_fd(server_socket_fd), socket_fd(socket_fd)
{

}

Client::~Client()
{

}

void	Client::setLastRequestMs(unsigned long last_request_ms)
{
	this->last_request_ms = last_request_ms;
}

void	Client::setStatus(t_status status)
{
	this->status = status;
	return ;
}

void	Client::setServerSocketFd(int server_socket_fd)
{
	this->server_socket_fd = server_socket_fd;
	return ;
}

void	Client::setSocketFd(int socket_fd)
{
	this->socket_fd = socket_fd;
	return ;
}

void	Client::setRemainBody(long long remain_body)
{
	this->remain_body = remain_body;
	return ;
}

t_status	Client::getStatus()
{
	return (this->status);
}

int			Client::getServerSocketFd()
{
	return (this->server_socket_fd);
}

Request		&Client::getRequest()
{
	return (this->request);
}

Response	&Client::getResponse()
{
	return (this->response);
}

int		Client::getSocketFd()
{
	return (this->socket_fd);
}

long long		Client::getRemainBody()
{
	return (this->remain_body);
}

unsigned long	Client::getLastRequestMs()
{
	return (this->last_request_ms);
}
/////////////////// class Client end ////////////////////
/////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////
////////////////// class Server start ///////////////////
Server::Server() : port(-1)
{
	
}

Server::Server(const Server& src)
{
	this->ip = src.ip;
	this->port	=	src.port;
	this->server_name	=	src.server_name;
	this->socket_fd		=	src.socket_fd;
	this->locations.insert(src.locations.begin(), src.locations.end());
}

Server &Server::operator=(const Server &src)
{
	this->ip = src.ip;
	this->port	=	src.port;
	this->server_name	=	src.server_name;
	this->socket_fd		=	src.socket_fd;
	this->locations.clear();
	this->locations.insert(src.locations.begin(), src.locations.end());

	return (*this);
}

Server::~Server()
{
	return ;
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

void	Server::setSocketFd(int socket_fd)
{
	this->socket_fd = socket_fd;
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

int					Server::getSocketFd() const
{
	return (this->socket_fd);
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
	this->cgi_extension = src.cgi_extension;
	this->auth_key = src.auth_key;
	this->redirect_addr = src.redirect_addr;
	this->redirect_return = src.redirect_return;
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
	this->cgi_extension = src.cgi_extension;
	this->auth_key = src.auth_key;
	this->redirect_addr = src.redirect_addr;
	this->redirect_return = src.redirect_return;
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

void		Location::setCgiExtension(const std::string &cgi_extension)
{
	this->cgi_extension = cgi_extension;
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

const std::string &Location::getCgiExtension()
{
	return (this->cgi_extension);
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
	std::cout << "cgi_extension	:	" << this->cgi_extension << std::endl;
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
}

///////////////// class Location end ////////////////////
/////////////////////////////////////////////////////////
