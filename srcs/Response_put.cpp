#include "config.hpp"
#include "response.hpp"
#include "nginx.hpp"

// int		Response::makeDirectoryToCreate(const Request& request, Location &location)
// {
// 	std::string absolute_path = location.getRoot(); // ./tests/test1_tester/test1_tester_root/put_test
// 	std::string url = request.getUri(); //  /put_test/a/b/c/number.txt

// 	// std::cout << url << std::endl;
// 	int status;
// 	if (*--absolute_path.end() == '/')
// 		absolute_path.erase(--absolute_path.end());
// 	if (url.find(location.getUriKey()) != std::string::npos)
// 		url = url.substr(location.getUriKey().length() - 1);
// 	// std::cout << url << std::endl;

// 	size_t offset = 0;
// 	size_t idx = 0;
// 	while(1)
// 	{
// 		if ((idx = url.find("/", offset + 1)) == std::string::npos)
// 			break ;
// 		absolute_path += url.substr(offset, idx - offset);
// 		// std::cout << absolute_path << std::endl;
// 		status = mkdir(absolute_path.c_str(), 0775);
// 		offset = idx;
// 	}
// 	return (0);
// }

// int		Response::createPutRequest(const Request& request, Location &location, int client_socket)
// {
// 	// 서버의 해당 url을 확인해서 파일의 존재여부를 판단
// 	// 존재 하지 않으면 create : status 201 (Created)
// 	// 존재 하면 : 내용 갈아 엎어 : status 200 (Ok)

// 	//여기서 만들기 직전에 addContentType 호출
// 	int fd;
// 	struct stat	sb;

// 	std::string absol_path = getAbsolutePath(request, location);
// 	// 존재 여부 판단
// 	// Path 존재시 1. directory >> 400 bad request return,
// 	//			2. file >> 200 ok : 파일 업데이트
// 	// 존재 안할 때 - 201 created : 파일 생성
// 	int	ret = 200;

// 	if (isDirectory(absol_path))
// 		return (400);
// 	else if (isExist(absol_path) == false)
// 	{
// 		ret = 201;
// 		makeDirectoryToCreate(request, location);
// 	}
// 	// 파일 open
// 	if ((fd = open(absol_path.c_str(), O_WRONLY | O_TRUNC | O_CREAT, 0777)) < 0)
// 		return (500);
// 	if (fstat(fd, &sb) < 0)
// 	{
// 		close(fd);
// 		return (500);
// 	}

// 	Resource *res = new Resource();
// 	res->setFd(fd);
// 	res->setFdWriteFrom(client_socket);
// 	Config::getInstance()->getNginx()->insert_pull(res);

// 	return (ret);
// }

// int		Response::makePutResponse(const Request& request, Location& location, int client_socket)
// {
// 	//여기까지 왔다면, auth 체크와 allowMethod 체크는 다 되어있다. 그리고 firstLine 은 200으로 작성되어있고, client 의 status 는 RESPONSE_READY 로 되어있다. 필요시 알아서 BODY_WRITING 으로 바꾸어준다.
// 	int ret;
// 	// 기본적인 헤더들 달아줌. (모두 실패하지 않는 것들)
// 	addServer();
// 	makeDate(request);
// 	addContentLanguage();
// 	addRetryAfter();
// 	addContentLength(0);
// 	this->raw_response += "\r\n";

// 	if ( (ret = createPutRequest(request, location, client_socket)) != 200 )
// 		return (makeErrorResponse(request, location, ret, client_socket));
// 	return (200);
// }
