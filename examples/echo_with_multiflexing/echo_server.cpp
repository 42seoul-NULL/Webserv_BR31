#include <iostream>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>

#include <unistd.h>

#include <fcntl.h>

#define BUF_SIZE 100

void    error_handling(const std::string &str)
{
    std::cerr << str << std::endl;
	printf("error %d : %s\n", errno,strerror(errno));
    exit(1);
}

// int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout)

// nfds: 검사 대상이 되는 파일 디스크립터의 수
// readfs: 읽기 이벤트를 검사할 파일 디스크립터의 목록
// writefds: 쓰기 이벤트를 검사할 파일 디스크립터의 목록
// exceptfds: 예외 이벤트를 검사할 파일 디스크립터의 목록
// timeout: 이벤트를 기다릴 시간 제한
// 반환 값: 이벤트가 발생한 파일의 갯수
// 반환 값이 이벤트가 발생한 파일의 디스크립터 목록이 아닌 파일의 갯수임에 주의해야합니다.

int     main(int ac, char **av)
{
    int					server_socket;
	int					client_socket;
	struct sockaddr_in  server_addr;
	struct sockaddr_in  client_addr;
	struct timeval		timeout;

	//파일 상태 테이블 선언
	fd_set	reads;
	fd_set	cpy_reads;

	fd_set	writes;
	fd_set	cpy_writes;

	socklen_t	addr_size;
	int		fd_max, str_len, fd_num;
	char	buf[BUF_SIZE];

	if (ac != 2)
		error_handling("plz, check your input");
	
	server_socket = socket(PF_INET, SOCK_STREAM, 0);
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(atoi(av[1]));

	if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
		error_handling("bind() error");
	if (listen(server_socket, 5) == -1)
		error_handling("listen() error");

	FD_ZERO(&reads); // fd_set 테이블을 초기화한다.
	FD_SET(server_socket, &reads); // 서버 소켓의 이벤트를 검사하기 위해 fd_set 테이블에 추가한다.

	fd_max = server_socket; // 이 부분은 전혀모르겠는데 ..? 띠용 
	
	while (1)
	{
		cpy_reads = reads;
		cpy_writes = writes;
		timeout.tv_sec = 5;
		timeout.tv_usec = 5000;
		//result
		// - 1 : 오류 발생
		// 0   : 타임 아웃
		// 1이상: 등록된 파일 디스크립터에 해당 이벤트가 발생하면 이벤트가 발생한 파일 디스크립터의 '수'를 반환

		if ( (fd_num = select(fd_max + 1, &cpy_reads, &cpy_writes, 0, &timeout)) == -1 )
		{
			error_handling("select error");
			break ;
		}
		
		if (fd_num == 0)
			continue ;

		for (int i=0; i<=fd_max; i++)
		{
			if (FD_ISSET(i, &cpy_reads)) // fd_set 테이블 검사
			{
				//서버소켓에 이벤트 발생
				if (i == server_socket) // connection request
				{
					addr_size = sizeof(client_addr);
					client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &addr_size);
					FD_SET(client_socket, &reads); // fd_set 테이블에 클라이언트 소켓 디스크립터를 추가한다.
					FD_SET(client_socket, &writes);
					if (fd_max < client_socket)
						fd_max = client_socket;
					printf("connected client : %d \n", client_socket);
				}
				else
				{
					str_len = read(i, buf, BUF_SIZE);
					if (str_len == 0) // close request
					{
						FD_CLR(i, &reads); // fd_set 테이블에서 파일 디스크립터를 삭제한다.
						close(i);
						printf("closed client: %d\n",i);
					}
				}
			}
			else if (FD_ISSET(i, &cpy_writes))
			{
				
			}
		}
	}
	close(server_socket);
	return (0);
}