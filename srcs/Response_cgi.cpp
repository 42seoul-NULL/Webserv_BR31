#include "webserv.hpp"

char	**Response::makeCgiEnv()
{
	char **ret;

	std::map<std::string, std::string>	temp_map;
	size_t								idx;
	std::map<std::string, std::string>	&headers = this->client->getRequest().getHeaders();

	std::string request_uri = this->client->getRequest().getUri();
	std::string path_info = request_uri.substr(request_uri.find(this->cgi_extention) + (this->cgi_extention.size()));
	std::string query_string;

	idx = path_info.find('?');
	if (idx != std::string::npos)
	{
		query_string = path_info.substr( idx + 1);
		path_info = path_info.substr(0, idx);
	}

	if (path_info == "")
		path_info += '/';

	if (headers.count(AUTHORIZATION) == 1)
	{
		idx = headers[AUTHORIZATION].find(' ');
		temp_map["AUTH_TYPE"] = headers[AUTHORIZATION].substr(0, idx);
		temp_map["REMOTE_USER"] = headers[AUTHORIZATION].substr(idx + 1);
		temp_map["REMOTE_IDENT"] = headers[AUTHORIZATION].substr(idx + 1);
	}
	temp_map["CONTENT_LENGTH"] = ft_itoa(this->client->getRequest().getRawBody().size());
	if (headers.count(CONTENT_TYPE) == 1)
		temp_map["CONTENT_TYPE"] = headers[CONTENT_TYPE];

	temp_map["GATEWAY_INTERFACE"] = "CGI/1.1";

	temp_map["PATH_INFO"] = path_info;
	temp_map["PATH_TRANSLATED"] = this->resource_path.substr(0, this->resource_path.find(cgi_extention) + cgi_extention.size());
	temp_map["QUERY_STRING"] = query_string;
	temp_map["REMOTE_ADDR"] = this->client->getServer()->getIP();
	temp_map["REQUEST_METHOD"] = this->client->getRequest().getMethod();
	if (cgi_extention == ".bla")
		temp_map["REQUEST_URI"] = path_info;
	else
		temp_map["REQUEST_URI"] = this->client->getRequest().getUri();

	temp_map["SCRIPT_NAME"] = this->location->getCgiInfos()[cgi_extention];

	temp_map["SERVER_NAME"] = this->client->getServer()->getServerName();
	temp_map["SERVER_PORT"] = ft_itoa(this->client->getServer()->getPort());
	temp_map["SERVER_PROTOCOL"] = "HTTP/1.1";
	temp_map["SERVER_SOFTWARE"] = "too_many_drivers_without_license";
	temp_map["REDIRECT_STATUS"] = "200";
	temp_map["SCRIPT_FILENAME"] = this->resource_path.substr(0, this->resource_path.find(cgi_extention) + cgi_extention.size());

	for (std::map<std::string, std::string>::iterator iter = headers.begin(); iter!= headers.end(); iter++)
		temp_map["HTTP_" + iter->first] = iter->second;
	if (!(ret = (char **)malloc(sizeof(char *) * (temp_map.size() + 1))))
		return (NULL);
	int i = 0;
	for (std::map<std::string, std::string>::iterator iter = temp_map.begin(); iter != temp_map.end(); iter++)
	{
		ret[i] = strdup((iter->first + "=" + iter->second).c_str());
		i++;
	}
	ret[i] = NULL;
	return (ret);
}

void	Response::makeCgiResponse()
{
	switch (this->client->getStatus())
	{
	case RESPONSE_MAKING:
	{
		//처음 왔다. 아무것도 준비되지 않았다.
		int fds[2];

		if ( (pipe(fds)) == -1 )  // fds[0] -> read ,   fds[1] -> write
			return (makeErrorResponse(500));
		this->client->setFdRead(fds[0]);
		this->client->setFdWrite(fds[1]);

		fcntl(fds[1], F_SETFL, O_NONBLOCK);
		setResource(fds[1], RAW_DATA_TO_FD, MAKE_RESPONSE, -1);

		mkdir("./temp", 0777);
		std::string temp_file_name = "./temp/tempfile_" + ft_itoa(this->client->getFdRead());
		int fd_temp = open(temp_file_name.c_str(), O_CREAT | O_TRUNC | O_RDWR, 0666);

		if (fd_temp == -1)
			return (makeErrorResponse(500));
		int pid = fork();
		if (pid == 0)
		{
			close(client->getFdWrite());
			dup2(client->getFdRead(), 0);
			dup2(fd_temp, 1);

			char *args[3];
			args[0] = strdup(this->location->getCgiInfos()[this->cgi_extention].c_str());
			args[1] = strdup((this->resource_path.substr(0, this->resource_path.find(cgi_extention) + cgi_extention.size())).c_str());
			args[2] = 0;

			char **cgi_env = makeCgiEnv();

			int ret;
			ret = execve(args[0], args, cgi_env);
			close(client->getFdRead());
			exit(ret);
		}
		else if (pid < 0) // fork error
			return (makeErrorResponse(500));
		else
		{
			Resource *res = new Resource(fd_temp, this->raw_response, this->client, FD_TO_RAW_DATA, MAKE_RESPONSE, -1);
			res->setPid(pid);
			res->setUnlinkPath(temp_file_name);
			this->resources.push_back(res);
			Config::getInstance()->getNginx()->insertToFdpool(res);
		}
		return ;
		break ;
	}
	case FILE_READ_DONE:
	{
		close(client->getFdRead());
		//close(client->getFdWrite());
		try
		{
			std::string first_line;
			size_t first_line_idx1 = this->raw_response.find("Status: ") + 8;
			size_t first_line_idx2 = this->raw_response.find("\r\n", first_line_idx1);

			first_line = "HTTP/1.1 " + this->raw_response.substr(first_line_idx1, first_line_idx2 - first_line_idx1) + "\r\n";

			// add date//
			time_t t;
			char buffer[4096];
			struct tm* timeinfo;

			t = time(NULL);
			timeinfo = localtime(&t);
			strftime(buffer, 4096, "%a, %d %b %Y %H:%M:%S GMT", timeinfo);
			first_line += "Date: " + std::string(buffer) + "\r\n";
			// add date //
			first_line += "Content-Language: ko\r\n";


			int content_size = this->raw_response.substr(this->raw_response.find("\r\n\r\n") + 4).size();

			this->raw_response = first_line +
								("Content-Length: " + ft_itoa(content_size) + "\r\n") +
								this->raw_response;

			this->client->setStatus(RESPONSE_MAKE_DONE);
		}
		catch(const std::exception& e)
		{
			return (makeErrorResponse(500));
		}
		break ;
	}
	default:
		break ;
	}
	return ;
}
