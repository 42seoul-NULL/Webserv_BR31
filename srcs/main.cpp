#include "webserv.hpp"

void sigIntHandler(int sig)
{
	sig = 0;

	std::cout << std::endl << ">> SIGINT CALLED <<" << std::endl;
	Config::getInstance()->getNginx()->cleanUp();
	exit(0);
}

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
		signal(SIGINT, sigIntHandler);
		nginx.run();
	}
	catch(const char *e)
	{
		std::cerr << e << '\n';
	}
}
