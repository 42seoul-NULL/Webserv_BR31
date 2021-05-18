#include "config.hpp"

config* config::instance;

///////////////////////////////////////////////////////////////
/////////////       config class start          ///////////////
///////////////////////////////////////////////////////////////
config::~config()
{
	delete this->instance;
}

config* config::getInstance()
{
	if (instance == NULL)
		instance = new config();
	return (instance);
}

config& config::operator=(const config& origin)
{
	return (*this);
}

///////////////// setter start ///////////////////
void	config::setPort(int value)
{
	this->port = value;
}

void	config::setIP(const std::string& ip)
{
	this->ip = ip;
}

void	config::setServerName(const std::string& server_name)
{
	this->server_name = server_name;
}
//////////////// setter end ////////////////

bool	config::makeConfig(const char *path)
{
	int		fd;
	char 	*line;
	int		ret;
	char	**split_ret;
	int		idx;

	fd = open(path, O_RDONLY);
	if (fd < 3)
		return (false);
	
	while ( (ret = get_next_line(fd, &line)) > 0 )
	{
		split_ret = ft_split(line, " \t");
		idx = -1;
		
		//parsing

		//


		idx = -1;
		while (split_ret[++idx] != 0)
			free(split_ret[idx]);
		free(split_ret);
		free(line);
	}
	free(line);


	//여기는 테스트를 위한 더미코드//

	server_name = "hyeonski_webServ";
	ip = "127.0.0.1";
	port = 3000;
	
	location loc;
	locations["/"] = loc;
	locations["/"].setRoot("./YoupiBanane");
	locations["/"].getIndex().push_back("index.html");
	locations["/"].getAllowMethods().push_back("GET");
	
	// 위 내용들을 파싱한다. libft.a 를 필수로 집어넣어야 할듯.
	

	return (true);
}





//////////////// getter start ////////////////
const std::string& config::getIP()
{
	return (this->ip);
}

const std::string& config::getServerName()
{
	return (this->server_name);
}

int config::getPort()
{
	return (this->port);
}

std::map<std::string, location>& config::getLocations()
{
	return (this->locations);
}
//////////////// getter end //////////////////
///////////////////////////////////////////////////////////////
/////////////       config class end            ///////////////
///////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////
/////////////       location class start        ///////////////
///////////////////////////////////////////////////////////////

location::location(const location& origin) : root(origin.root), index(origin.index), allow_methods(origin.allow_methods), client_body_buffer_size(origin.client_body_buffer_size)
{
}

location& location::operator=(const location& origin)
{
	this->root = origin.root;
	index.assign(origin.index.begin(), origin.index.end());
	allow_methods.assign(origin.allow_methods.begin(), origin.allow_methods.end());
	this->client_body_buffer_size = origin.client_body_buffer_size;
	return (*this);
}

////////////////////////	setter start	//////////////////////////
void		location::setRoot(const std::string& root)
{
	this->root = root;
}

void		location::setClientBodyBufferSize(unsigned int value)
{
	this->client_body_buffer_size = value;
}

////////////////////////	setter end		//////////////////////////


////////////////////////	getter start	//////////////////////////
const std::string& location::getRoot()
{
	return (this->root);
}

std::list<std::string>& location::getIndex()
{
	return (this->index);
}

std::list<std::string>& location::getAllowMethods()
{
	return (this->allow_methods);
}

const	unsigned int	location::getClientBodyBufferSize()
{
	return (this->client_body_buffer_size);
}

////////////////////////	getter end		//////////////////////////