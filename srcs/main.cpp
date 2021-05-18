#include "../libft_cpp/libft.hpp"
#include "../includes/parser.hpp"
#include "../includes/nginx.hpp"

int	main(int ac, char **av)
{
	ac = 0;

	if(!Config::getInstance()->makeConfig(av[1]))
		return (1);
	//Config::getInstance()->show();

	Nginx nginx;
	struct timeval		timeout;

	timeout.tv_sec = 5; // last request time out 5000ms
	timeout.tv_usec = 0;

	try
	{
		nginx.initServers(5);
		nginx.run(timeout, 100);
	}
	catch(const char *e)
	{
		std::cerr << e << '\n';
	}
}
