#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>

#define BUF_SIZE 1024
void error_handling(char *message);

int     main(int ac, char **av)
{
    //파일 디스크립터를 위한 변수
    int		server_socket;
    int		client_socket;
    char	message[BUF_SIZE];
    int		str_len;
    int		i;

    // struct sockaddr_in
    // {
    //     sa_family_t //  sin_family // 주소체계 (address family)
    //     uint16_t    //  sin_port // 16비트 TCP /UDP port
    //     struct in_addr  //  sin_addr // 32비트 IPv4 주소
    //     char        //  sin_zero[8] // 사용되지않음
    // }

    //위 생소한 데이터타입들은 POSIX 에서 근거를 찾을 수 있다.
    //구조체의 sin_family 변수에는 주소체계정보를 대입한다.
    //AF_INET   IPv4 인터넷 프로토콜
    //AF_INET6  IPv6 인터넷 프로토콜
	//AF_LOCAL  Local 통신을 위한 UNIX 프로토콜

    //sockaddr_in 구조체 변수에 값을 대입 할 경우에는 반드시 네트워크바이트 순서로 값을 변경한 다음 대입해야한다.
	//Big_Endian 방식(네트워크 바이트 순서)
	//상위 바이트의 값이 메모리상먼저 표시되는 방법

	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
    socklen_t client_addr_size;

    if (ac != 2)
    {
        printf("plz, check your input\n");
        return (1);
    }

    //socket(int domain, int type, int protcol)
    //1. PF_INET => IPv4 인터넷 프로토콜
    //2. SOCK_STREAM => 연결지향형 (TCP)
    //3. 사실 첫번째와 두번쨰 인자만 잘 정해졌어도 세번째 인자에 0이들어와도 적합한 소켓이 생성 될 수 있다.
    //  그렇다면 언제 사용하는가? 바로 하나의 프로토콜체계안에서 최종적으로 통시하는 형태가 다른 여러개의 프로토콜이 존재하는 경우에 한해서 이다.
    //  RAW_SOCKET 을 생성하는 경우가 유용한 경우라고 하는데, 일단은 스킵하자.
    //  대체로 socket(PF_INET, SOCK_STREAM, ) 인 경우에는 세번째 인자가 TCP 타입이,
    //  socket(PF_INET, SOCK_DGRAM, )인 경우에는 세번쨰 인자로 UDP타입이 들어간다.
    
	
	server_socket = socket(PF_INET, SOCK_STREAM, 0);
	//server_socket = socket(AF_INET, SOCK_STREAM, 0);
	//AF_INET , PF_INET	둘다 정상적으로 동작한다. 차이는 ? => 없다. #define PF_INET         AF_INET
	//AF 는 Address Family, PF는 Protocol Family 로, 각각 '주소체계'를 정의할때 쓰이는지,
	//'프로토콜'을 정의할때 쓰는지에 따라 prefix 를 다르게 만들어 놓은 것이다.
	// 사실상 무의미하다. https://www.bangseongbeom.com/af-inet-vs-pf-inet.html

	if (server_socket == -1)
		error_handling("socket() error");

	//소켓 생성
	memset(&server_addr, 0, sizeof(server_addr));
	
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	//server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    server_addr.sin_port = htons(atoi(av[1]));

	//socket 에 ip와 port 번호를 할당
	if (bind(server_socket, (struct sockaddr*)(&server_addr), sizeof(server_addr)) == -1)
		error_handling("bind() error");
	
	//server socket 을 통해 클라이언트의 접속 요청을 대기 5개의 수신대기열(큐)를 생성
	if (listen(server_socket, 5) == -1)
		error_handling("listen() error");

	client_addr_size = sizeof(client_addr);

	for (int i=0; i < 5; i++)
	{
		//클라이언트 접속 요청을 수락. 클라이언트와 연결된 새로운 소켓이 생성된다.
		client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_size );
		if (client_socket == -1)
			error_handling("accept() error");
		else
			printf("Connected client %d \n", i+ 1);
		
		//클라이언트와 연결된 소켓을통해 데이터 송수신한다.
		//
		while ((str_len = read(client_socket, message, BUF_SIZE)) != 0)
		{
			write(1, message, str_len);
			write(client_socket, message, str_len);
		}

		printf("read : %d\n", str_len);
		//111번 라인에서 무한루프를 돌기 때문에 다른 클라이언트의 요청을 처리 할수 없는 상태가 된다.
		//printf("hello\n");
		close(client_socket);
	}
	close(server_socket);
	return (0);

}

void error_handling(char *message)
{
	printf("%s\n", message);
	printf("error %d : %s\n", errno,strerror(errno));
	exit(1);
}