#include "webserv.hpp"

 //REQUEST_RECEIVING,
	//RESPONSE_MAKING,
	//FILE_READING,
	//FILE_READ_DONE,
	//FILE_WRITING,
	//FILE_WRITE_DONE,
	//RESPONSE_MAKE_DONE

void		Response::makeHeadResponse()
{
    switch (this->client->getStatus())
    {
    case RESPONSE_MAKING:
    {
        //기본적인 헤더부터 공사해준다.
        addFirstLine(200);
        addDate();
        addContentLanguage();

        //여기서 만들기 직전에 addContentType 호출
        int fd;
        struct stat	sb;
        size_t idx;

        if (isDirectory(this->resource_path))
        {
            if (*(--this->resource_path.end()) != '/')
                this->resource_path += '/';

            bool is_exist = false;
            std::string temp_path;
            for (std::list<std::string>::iterator iter = this->location->getIndex().begin(); iter != this->location->getIndex().end(); iter++)
            {
                temp_path = (this->resource_path + (*iter));
                is_exist = isExist(temp_path);
                if ( is_exist == true )
                    break ;
            }
            if (is_exist == false && this->location->getAutoIndex())
            {
				makeAutoIndexPage();
                this->raw_response = this->raw_response.substr(0, this->raw_response.find("\r\n\r\n") + 4);
                return ;
			}
            this->resource_path = temp_path;
        }
        if (!isExist(this->resource_path))
		{
			makeErrorResponse(404);
			this->raw_response = this->raw_response.substr(0, this->raw_response.find("\r\n\r\n") + 4);
            return ;
		}

        idx = this->resource_path.find_first_of('/');
        idx = this->resource_path.find_first_of('.',idx);

        if (idx == std::string::npos) // 확장자가 없다.
            addContentType(".bin");
        else
            addContentType(this->resource_path.substr(idx));

        if ((fd = open(this->resource_path.c_str(), O_RDONLY)) < 0)
		{
			makeErrorResponse(500);
			this->raw_response = this->raw_response.substr(0, this->raw_response.find("\r\n\r\n") + 4);
            return ;
		}
        if (fstat(fd, &sb) < 0)
        {
            close(fd);
        	makeErrorResponse(500);
			this->raw_response = this->raw_response.substr(0, this->raw_response.find("\r\n\r\n") + 4);
            return ;
		}
        addContentLength((int)sb.st_size);
        addEmptyline();
        this->client->setStatus(RESPONSE_MAKE_DONE);

        break ;
	}
    default:
        break ;
    }
}
