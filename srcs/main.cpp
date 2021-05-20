#include "libft.hpp"
#include "config.hpp"
#include "nginx.hpp"

int	main(int ac, char **av)
{
	ac = 0;

	if(!Config::getInstance()->makeConfig(av[1]))
		return (1);
	//Config::getInstance()->show();
	//return 0;

	Nginx nginx;
//	struct timeval		timeout;

	// timeout.tv_sec = 5; // last request time out 5000ms
	// timeout.tv_usec = 0;
	try
	{
		nginx.initServers();
		nginx.run();
	}
	catch(const char *e)
	{
		std::cerr << e << '\n';
	}
}
