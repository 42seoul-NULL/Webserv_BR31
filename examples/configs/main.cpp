#include "config.hpp"
#include <stdlib.h>

int		main()
{
	config::getInstance()->makeConfig("config_exam.conf");

	std::cout << "server_name : " << config::getInstance()->getServerName() << std::endl;
	std::cout << "server_ip : " << config::getInstance()->getIP() << std::endl;
	std::cout << "server_port : " << config::getInstance()->getPort() << std::endl;

	std::cout << "location info : " << config::getInstance()->getLocations().begin()->first << std::endl;
	std::cout << config::getInstance()->getLocations()["/"].getRoot() << std::endl;
	std::cout << config::getInstance()->getLocations()["/"].getIndex().front() << std::endl;

	system("leaks a.out | grep leaked");
}