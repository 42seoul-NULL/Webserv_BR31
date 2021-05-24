#include "webserv.hpp"


int	main(int ac, char **av)
{
	ac = 0;

	if(!Config::getInstance()->makeConfig(av[1]))
		return (1);
	//Config::getInstance()->show();
	//return 0;

	Nginx nginx;
	Config::getInstance()->setNginx(&nginx);

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
