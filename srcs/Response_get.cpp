#include "config.hpp"
#include "response.hpp"
#include "nginx.hpp"

void		Response::makeAutoIndexPage()
{
	this->raw_response.clear();
	std::string body;
	std::string pre_addr = "http://" + this->client->getRequest().getHeaders()[HOST] + "/";

	body += "<!DOCTYPE html>";
	body += "<html>";
	body += "<head>";
	body += "</head>";
	body += "<body>";
	body += "<h1> AutoIndex : "+ this->client->getRequest().getUri() +"</h1>";

	DIR *dir = NULL;
	struct dirent *file = NULL;
	if ( (dir = opendir(this->resource_path.c_str())) == NULL )
		return (makeErrorResponse(500));
	while ( (file = readdir(dir)) != NULL )
	{
		std::string file_name(file->d_name);
		if (file_name != "." && file_name != "..")
			body += "<a href=\"" + pre_addr + file_name + "\">" + file_name + "</a><br>";
	}
	closedir(dir);

	body += "";
	body += "";
	body += "</body>";
	body += "</html>";

	addFirstLine(200);
	addDate();
	this->raw_response += "Content-Type: " + Config::getInstance()->getMimeType()[".html"] + "\r\n";
	this->raw_response += "Content-Length: " + ft_itoa(body.size()) + "\r\n";
	this->raw_response += "\r\n";
	this->raw_response += body;
    this->client->setStatus(RESPONSE_MAKE_DONE);
}

void		Response::makeGetResponse()
{
    //REQUEST_RECEIVING,
	//RESPONSE_MAKING,
	//FILE_READING,
	//FILE_READ_DONE,
	//FILE_WRITING,
	//FILE_WRITE_DONE,
	//RESPONSE_MAKE_DONE

    switch (this->client->getStatus())
    {
    case RESPONSE_MAKING:
    {
        //여기서 만들기 직전에 addContentType 호출
        int fd;
        struct stat	sb;
        size_t idx;

        if (isDirectory(this->resource_path))
        {
            bool is_exist = false;
            std::string temp_path;
            for (std::list<std::string>::iterator iter = this->location->getIndex().begin(); iter != this->location->getIndex().end(); iter++)
            {
                temp_path = (this->resource_path + (*iter));
                if ((is_exist = isExist(temp_path)) == true)
                    break ;
            }
            if (is_exist == false && this->location->getAutoIndex())
                return (makeAutoIndexPage());
        }
        if (!isExist(this->resource_path))
            return (makeErrorResponse(404));

        idx = this->resource_path.find_first_of('/');
        idx = this->resource_path.find_first_of('.',idx);

        if (idx == std::string::npos) // 확장자가 없다.
            addContentType(".bin");
        else
            addContentType(this->resource_path.substr(idx));

        if ((fd = open(this->resource_path.c_str(), O_RDONLY)) < 0)
            return (makeErrorResponse(500));
        if (fstat(fd, &sb) < 0)
        {
            close(fd);
            return (makeErrorResponse(500));
        }
        addContentLength((int)sb.st_size);
        this->raw_response += "\r\n";

        setResource(fd, FD_TO_RAW_DATA, MAKE_RESPONSE);
        return ;
        break;
    }
    case FILE_READ_DONE:
    {
        this->client->setStatus(RESPONSE_MAKE_DONE);
        break ;
    }
    default:
        break;
    }
}