#include "../includes/parser.hpp"
#include "../includes/Response.hpp"
#include "../includes/nginx.hpp"

bool	Response::isCgiExtension(const Request& request, Location& location)
{
	for (std::map<std::string, std::string>::iterator iter = location.getCgiInfos().begin(); iter != location.getCgiInfos().end(); iter++)
	{
		if (request.getUri().find(iter->first))
			return (true);
	}
	return (false);
}

char	**Response::makeEnv(const Request& request, Location& location)
{
	(void)location;
	std::map<std::string, std::string> tmap;
	tmap["CONTENT_LENGTH"]=ft_itoa(request.getRawBody().size());
	tmap["GATEWAY_INTERFACE"]="CGI/1.1";
	tmap["HTTP_Accept"]="*/*";
	tmap["HTTP_Connection"]="keep-alive";
	tmap["HTTP_Host"]=request.getHost();
	tmap["PATH_INFO"]="/";
	tmap["REQUEST_URI"]="/";
	tmap["PATH_TRANSLATED"]="/";
	tmap["REMOTE_ADDR"]="0.0.0.0";
	tmap["REQUEST_METHOD"]=request.getMethod();
	tmap["SCRIPT_NAME"]="./cgi_tester";
	tmap["SERVER_NAME"]="localhost";
	tmap["SERVER_PORT"]="8080";
	tmap["SERVER_PROTOCOL"]="HTTP/1.1";
	tmap["SERVER_SOFTWARE"]="hski";

	char **ret = (char **)malloc((tmap.size() + 1) * sizeof(char *));
	int idx = 0;
	for (std::map<std::string, std::string>::iterator iter = tmap.begin(); iter != tmap.end(); iter++)
		ret[idx] = strdup((iter->first+"="+iter->second).c_str());
	ret[idx] = 0;
	return (ret);
}

int		Response::makeCgiResponse(const Request& request, Location& location, int client_socket, int cgi_stdin_fd)
{
	(void)request;
	(void)location;

	Client *cli = dynamic_cast<Client *>(Config::getInstance()->getNginx()->getFds()[client_socket]);

	// 여기까지 왔다면 allow method 와 first line 은 세팅이 되어있다.

	if (cgi_stdin_fd == -1) // cgi 의 stdin으로 들어갈 놈이 준비가 안되어있다.
	{
		int		fds[2]; // 파이프를 뜨면 fds[1] 에 써주고, fds[0] 에서 읽어주면 된다.
		if (pipe(fds) == -1)
			return (500);
		Pipe	*pip = new Pipe();
		pip->setPipeRead(fds[0]);
		pip->setPipeWrite(fds[1]);
		pip->setWriteFromClient(client_socket);
		Config::getInstance()->getNginx()->insert_pull(pip);

		cli->setStatus(BODY_WRITING);
		return (200);
	}
	else // 잘 담겨왔다.
	{
		int		fds[2]; // 파이프를 뜨면 fds[0] 에서 읽어주면된다. 자식은 fds[1] 에 써주면됨.
		if (pipe(fds) == -1)
			return (500);

		int ret = fork();
		if (ret == 0)
		{
			//child
			dup2(cgi_stdin_fd, 0);
			dup2(fds[1], 1);
		}
		else
		{
			//parent
		}
	}
	return (200);	
}