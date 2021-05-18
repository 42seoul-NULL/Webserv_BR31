#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>


#define BUF_SIZE 1024
void error_handling(char *message)
{
    printf("%s\n", message);
    exit(1);
}

int main(int ac, char **av)
{
	int		sock;
	char	message[BUF_SIZE];
	int		str_len;
	struct sockaddr_in	server_addr;

	if (ac != 3)
		error_handling("plz, check your argument");

	//socket을 하나 생성한다.
	sock = socket(PF_INET, SOCK_STREAM, 0);
	if (sock == -1)
		error_handling("socket() error");
	
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(av[1]);
	server_addr.sin_port = htons(atoi(av[2]));

	//socket 을 이용해 server의 server socket 에 연결을 요청한다.
	if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
		error_handling("connect() error!");
	else
		printf("Connected..............\n");
	
	while (1)
	{
		fputs("Input message (Q to quit) : ", stdout);
		fgets(message, BUF_SIZE, stdin);
		
		if (!strcmp(message, "q\n") || !strcmp(message, "Q\n"))
			break ;		
		write(sock, message, strlen(message));
		str_len = read(sock, message, BUF_SIZE - 1);
		message[str_len] = 0;
		printf("Message from server : %s", message);
	}
	close(sock);
	return (0);
}
