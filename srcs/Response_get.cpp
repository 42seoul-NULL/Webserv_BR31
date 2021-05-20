#include "config.hpp"
#include "response.hpp"
#include "nginx.hpp"

int		Response::makeGetBody(const Request& request, Location &location, int client_socket)
{
	//여기서 만들기 직전에 makeContentType 호출
	int fd;
	struct stat	sb;
	size_t idx;

	std::string absol_path = getAbsolutePath(request, location);

	std::cout << request.getUri() << std::endl;
	std::cout << location.getUriKey() << std::endl;
	std::cout << absol_path << std::endl;

	if (isDirectory(absol_path))
	{
		if ( *(--absol_path.end()) != '/')
			absol_path += '/';

		bool is_exist = false;
		std::string temp_path;
		for (std::list<std::string>::iterator iter = location.getIndex().begin(); iter != location.getIndex().end(); iter++)
		{
			temp_path = (absol_path + (*iter));
			if ((is_exist = isExist(temp_path)) == true)
				break ;
		}
		if (is_exist == false && location.getAutoIndex())
			return (makeAutoIndexPage(request, absol_path));
		absol_path = temp_path;
	}
	if (!isExist(absol_path))
		return (404);

	idx = absol_path.find_first_of('/');
	idx = absol_path.find_first_of('.',idx);

	if (idx == std::string::npos) // 확장자가 없다.
		makeContentType(request, "application/octet-stream");
	else
		makeContentType(request, Config::getInstance()->getMimeType()[absol_path.substr(idx)]);

	if ((fd = open(absol_path.c_str(), O_RDONLY)) < 0)
		return (500);
	if (fstat(fd, &sb) < 0)
	{
		close(fd);
		return (500);
	}
	makeContentLength((int)sb.st_size);
	makeLastModified(request, location);
	this->raw_response += "\r\n";

	Resource *resrc = new Resource();
	resrc->setFd(fd);
	resrc->setFdReadTo(client_socket);
	Config::getInstance()->getNginx()->insert_pull(resrc);
	(dynamic_cast<Client *>(Config::getInstance()->getNginx()->getFds()[client_socket]))->setStatus(BODY_WRITING);
	return (200);
}

int		Response::makeGetResponse(const Request& request, Location& location, int client_socket)
{
	//여기까지 왔다면, auth 체크와 allowMethod 체크는 다 되어있다. 그리고 firstLine 은 200으로 작성되어있고, client 의 status 는 RESPONSE_READY 로 되어있다. 필요시 알아서 BODY_WRITING 으로 바꾸어준다.
	int ret;

	// 기본적인 헤더들 달아줌. (모두 실패하지 않는 것들)
	makeContentLanguage();
	makeContentLocation(request, location);
	makeDate(request);
	makeRetryAfter();
	makeServer();

	if ( (ret = makeGetBody(request, location, client_socket)) != 200 )
		return (makeErrorReponse(request, location, ret, client_socket));
	return (200);
}

