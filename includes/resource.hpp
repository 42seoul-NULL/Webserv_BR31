#ifndef RESOURCE_HPP
# define RESOURCE_HPP

# include <string>
# include <map>
# include <list>
# include <sys/wait.h>
# include "fdmanager.hpp"
# include "client.hpp"

/*
Resource 책임/권한
- 서버의 리소스를 읽거나, 서버에 리소스를 추가하거나 하는데 사용하는 fdmanger 객체
- select 을 통해 리소스의 fd가 선택되면, 
1. isReady 찍어서 확인 
	NginX 가
	READY -> nginx가 fd 읽던 쓰던 할일함
		할일이 끝나면, doNext를 호출; -> delete_from_pool()
	NOT_YET -> 대기(즉 NginX fd가 준비안되었다고 판단)
	CGI_CRASH -> 클라이언트의 리스폰스 init해서 밀어버리고, client->response->makeErrorResponse(500); delete_from_pool(이거);
*/
class Resource	:	public Fdmanager
{
	private :
		std::string &raw_data;
		int			pid;
		int	response_error_num;
		Client		*client;

		e_direction	direction;
		e_nextcall	next_call;

	public	:
		Resource(int fd, std::string& raw_data, Client *client, e_direction direction, e_nextcall next_call, int response_error_num);
		virtual ~Resource();

		//method
		e_resource_is_ready_status isReady();
		void		doNext();
		bool		isFdToRawData();
		bool		isRawDataToFd();

		//setter
		void		setResponseErrorNum(int response_error_num);

		//getter
		std::string &getRawData();
		Client		*getClient();
};

#endif