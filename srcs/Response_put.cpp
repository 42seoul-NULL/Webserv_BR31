#include "webserv.hpp"
/*
1. client_status 에 따라 처리
- RESPONSE_MAKING
	> 해당 URL이 서버에 있는 directory 다
		(400) bad request
	> 해당 URL이 디렉토리가 아니다
		> 해당 URL이 존재 하지 않는다.
			(201) Created.
		> 해당 URL이 존재 한다.
			(204) No content
		> Resouce set - and go~
- FILE_WRITE_DONE
	> RESPONSE_MAKE_DONE
> 헤더 만들기
*/
void		Response::makePutResponse()
{
	switch (this->client->getStatus())
	{
		case RESPONSE_MAKING:
		{
			if (isDirectory(this->resource_path))
			{
				makeErrorResponse(400);
				return ;
			}
			else
			{
				if (isExist(this->resource_path))
					addFirstLine(204);
				else
				{
					addFirstLine(201);
					mkdirResourcePath();
				}
				int fd = open(this->resource_path.c_str(), O_WRONLY | O_TRUNC | O_CREAT, 0777);
				if (fd < 0)
				{
					close(fd);
					makeErrorResponse(500);
					return ;
				}
				addDate();
				//addContentLength(0);
				addEmptyline();
				setResource(fd, RAW_DATA_TO_FD, MAKE_RESPONSE);
			}
			break ;
		}
		case FILE_WRITE_DONE:
		{
			this->client->setStatus(RESPONSE_MAKE_DONE);
			break ;
		}
		default:
			break ;
	}
}

int		Response::mkdirResourcePath()
{
	std::string	url = this->client->getRequest().getUri(); //  /put_test/a/b/c/number.txt
	std::string	copy_resource_path = this->location->getRoot();
	int status;
	if (*--copy_resource_path.end() == '/')
		copy_resource_path.erase(--copy_resource_path.end());
	if (url.find(location->getUriKey()) != std::string::npos)
		url = url.substr(location->getUriKey().length() - 1);
	size_t offset = 0;
	size_t idx = 0;
	while(1)
	{
		if ((idx = url.find("/", offset + 1)) == std::string::npos)
			break ;
		copy_resource_path += url.substr(offset, idx - offset);
		status = mkdir(copy_resource_path.c_str(), 0777);
		offset = idx;
	}
	return (0);
}
