#include "config.hpp"
#include "response.hpp"
#include "nginx.hpp"

int		Response::makeGetResponse()
{

}

// int		Response::makeGetBody(const Request& request, Location &location, int client_socket)
// {
// 	//여기서 만들기 직전에 addContentType 호출
// 	int fd;
// 	struct stat	sb;
// 	size_t idx;

// 	std::string absol_path = getAbsolutePath(request, location);

// 	std::cout << request.getUri() << std::endl;
// 	std::cout << location.getUriKey() << std::endl;
// 	std::cout << absol_path << std::endl;

// 	if (isDirectory(absol_path))
// 	{
// 		if ( *(--absol_path.end()) != '/')
// 			absol_path += '/';

// 		bool is_exist = false;
// 		std::string temp_path;
// 		for (std::list<std::string>::iterator iter = location.getIndex().begin(); iter != location.getIndex().end(); iter++)
// 		{
// 			temp_path = (absol_path + (*iter));
// 			if ((is_exist = isExist(temp_path)) == true)
// 				break ;
// 		}
// 		if (is_exist == false && location.getAutoIndex())
// 			return (makeAutoIndexPage(request, absol_path));
// 		absol_path = temp_path;
// 	}
// 	if (!isExist(absol_path))
// 		return (404);

// 	idx = absol_path.find_first_of('/');
// 	idx = absol_path.find_first_of('.',idx);

// 	if (idx == std::string::npos) // 확장자가 없다.
// 		addContentType(request, "application/octet-stream");
// 	else
// 		addContentType(request, Config::getInstance()->getMimeType()[absol_path.substr(idx)]);

// 	if ((fd = open(absol_path.c_str(), O_RDONLY)) < 0)
// 		return (500);
// 	if (fstat(fd, &sb) < 0)
// 	{
// 		close(fd);
// 		return (500);
// 	}
// 	addContentLength((int)sb.st_size);
// 	addLastModified(request, location);
// 	this->raw_response += "\r\n";

// 	Resource *resrc = new Resource();
// 	resrc->setFd(fd);
// 	resrc->setFdReadTo(client_socket);
// 	Config::getInstance()->getNginx()->insert_pull(resrc);
// 	(dynamic_cast<Client *>(Config::getInstance()->getNginx()->getFds()[client_socket]))->setStatus(BODY_WRITING);
// 	return (200);
// }

