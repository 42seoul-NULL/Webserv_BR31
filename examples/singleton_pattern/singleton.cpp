//싱글톤을 어느때 적절히 사용할수 있냐 하면,
//하나의 프로그램 내에서 공통적으로 쓰이는 자원을 관리, 저장하는 역할을 할 때
//즉, 이번 프로젝트에서는 config 파일을 읽어서 기초 자료를 메모리화 해서 가지고 있어야 할 것 같으므로
//config class 를 싱글톤으로 디자인하여 관리하면 어디서나 config파일의 자료를 접근 할 수 있기
//때문에 편하지 않을까 한다.

#include "singleton.hpp"
#include <iostream>


void	changeValue()
{
	config::getInstance()->setPort(500);
}

int		main()
{
	std::cout << config::getInstance()->getPort() << std::endl;

	config::getInstance()->setPort(300);

	std::cout << config::getInstance()->getPort() << std::endl;

	changeValue();

	std::cout << config::getInstance()->getPort() << std::endl;
}