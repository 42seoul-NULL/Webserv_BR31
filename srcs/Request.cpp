#include "request.hpp"
#include "client.hpp"

Request::Request(void)
{
	this->raw_request.clear();
	initRequest();
}

///////////////// public func ///////////////////////
void	Request::initRequest(void)
{
	this->method.clear();
	this->uri.clear();
	this->http_version.clear();
	this->headers.clear();
	this->raw_body.clear();
	this->temp_body.clear();
	body_receving = false;
	body_length_info_type = NOTHING;
}

e_request_try_make_request_return	Request::tryMakeRequest(void)
{
	std::size_t	found = this->raw_request.find("\r\n\r\n");
	int	res = 1;

	if (found != std::string::npos && this->body_receving == 0)
	{
		this->makeStartLine();
		this->makeRequestHeader();
		this->body_receving = true;
		res = bodyCheck();
		if (res == 0)
		{
			this->raw_request = this->temp_body;
			this->temp_body.clear();
			return (requestValidCheck(true));
		}
	}
	if (this->body_receving == true)
	{
		this->temp_body += raw_request;
		return (requestValidCheck(isComplete()));
	}
	return (requestValidCheck(false));
}

///////// getter ////////////
std::string&	Request::getRawRequest(void)
{
	return (this->raw_request);
}

const std::string&	Request::getMethod(void) const
{
	return (this->method);
}

const std::string&	Request::getUri(void) const
{
	return (this->uri);
}

const std::string&	Request::getHttpVersion(void) const
{
	return (this->http_version);
}

std::map<std::string, std::string>&	Request::getHeaders(void) const
{
	return (const_cast<std::map<std::string, std::string>& > (this->headers));
}

const	std::string&	Request::getRawBody(void) const
{
	return (this->raw_body);
}

///////////////// private func ///////////////////////
int		 	Request::base64_decode(const char * text, char * dst, int numBytes)
{
    const char* cp;
    int space_idx = 0, phase;
    int d, prev_d = 0;
    char c;
    space_idx = 0;
    phase = 0;
    for (cp = text; *cp != '\0'; ++cp) {
        d = Config::decodeMimeBase64[(int)*cp];
        if (d != -1) {
            switch (phase) {
            case 0:
                ++phase;
                break;
            case 1:
                c = ((prev_d << 2) | ((d & 0x30) >> 4));
                if (space_idx < numBytes)
                    dst[space_idx++] = c;
                ++phase;
                break;
            case 2:
                c = (((prev_d & 0xf) << 4) | ((d & 0x3c) >> 2));
                if (space_idx < numBytes)
                    dst[space_idx++] = c;
                ++phase;
                break;
            case 3:
                c = (((prev_d & 0x03) << 6) | d);
                if (space_idx < numBytes)
                    dst[space_idx++] = c;
                phase = 0;
                break;
            }
            prev_d = d;
        }
    }
    return space_idx;
}

e_request_try_make_request_return	Request::requestValidCheck(bool isComplete)
{
	if (isComplete == false)
		return (WAITING_REQUEST_MSG);
	else
	{
		Location &loc = this->client->getServer()->getPerfectLocation(this->uri);
		//set response location
		this->client->getResponse().setLocation(&loc);

		//auth check
		if (isValidAuthHeader(loc) == false)
			return (I_MAKE_ERROR_RESPONSE);
		//allow_method check
		if (isValidMethod(loc) == false)
			return (I_MAKE_ERROR_RESPONSE);

		//set response resource_path
		std::string resource_path = loc.getRoot() + this->uri.substr(loc.getUriKey().size());

		this->client->getResponse().setResourcePath(resource_path);
		
		//set response cgi_extention
		for (std::map<std::string, std::string>::iterator iter = loc.getCgiInfos().begin(); iter != loc.getCgiInfos().end(); iter++)
		{
			if (resource_path.find(iter->first) != std::string::npos) // cgi_extention 표현을 찾았다면
			{
				this->client->getResponse().setCgiExtention(iter->first);
				break ;
			}
		}
	}
}

void	Request::makeStartLine(void)
{
	this->parseMethod();
	this->parseUri();
	this->parseHttpVersion();
	std::size_t n = this->raw_request.find("\r\n");
	if (this->raw_request.length() >= (n + 2))
		this->raw_request = this->raw_request.substr(n + 2);
	else
		this->raw_request = "";
}

void	Request::parseMethod(void)
{
	std::size_t	found = this->raw_request.find("\r\n");
	std::string start_line = this->raw_request.substr(0, found);
	this->method = start_line.substr(0, start_line.find(' '));
}

void	Request::parseUri(void)
{
	std::size_t	found = this->raw_request.find("\r\n");
	std::string start_line = this->raw_request.substr(0, found);
	std::size_t start_pos = start_line.find(' ');

	this->uri = start_line.substr(start_pos + 1, start_line.find_last_of(' ') - start_pos - 1);
}

void	Request::parseHttpVersion(void)
{
	std::size_t	found = this->raw_request.find("\r\n");
	std::string start_line = this->raw_request.substr(0, found);

	this->http_version = start_line.substr(start_line.find_last_of(' ') + 1);
}

void	Request::makeRequestHeader(void)
{
	std::string raw_header = this->raw_request.substr(this->raw_request.find("\r\n") + 1, this->raw_request.find("\r\n\r\n"));
	
	std::vector<std::string> split_vec;

	ft_split(raw_header, "\r\n", split_vec);
	std::vector<std::string>::iterator i;
	for (i = split_vec.begin(); i != split_vec.end(); i++)
	{
		std::string temp = *i;
		std::size_t found = temp.find(":");
		std::string key = temp.substr(0, found);
		while (temp[found + 1] == 32)
			found++;
		std::string value = "";
		if (temp.length() != (found + 1))
			value = temp.substr(found + 1);
		headers[key] = value;
	}

	// 맵 출력용
	// for (std::map<std::string, std::string>::iterator j = headers.begin(); j != headers.end(); j++)
	// 	std::cout << "[" << j->first << "] value = [" << j->second << "]" << std::endl;

	this->temp_body = this->raw_request.substr(this->raw_request.find("\r\n\r\n") + 4);
	this->raw_request.clear();
}

bool	Request::bodyCheck(void)
{
	if (this->headers["Transfer-Encoding"] == "chunked")
	{
		this->body_length_info_type = CHUNKED;
		return (true);
	}
	else if (this->headers["Content-Length"] != "")
	{
		this->body_length_info_type = CONTENT_LENGTH;
		return (true);
	}
	return (false);
}

bool	Request::isComplete(void)
{
	std::size_t len = ft_atoi(this->headers["Content-Length"]);

	if (this->body_length_info_type == CONTENT_LENGTH && this->temp_body.length() >= len)
	{
		this->raw_body += this->temp_body.substr(0, len);
		this->raw_request += this->temp_body.substr(len); // 다음 리퀘스트가 한 번에 붙어서 오면 어떻게 처리해야하는가?
		temp_body.clear();
		return (true);
	}
	else if (this->body_length_info_type == CHUNKED)
	{
		std::size_t found = this->temp_body.find("\r\n");
		std::size_t	chunk_size;

		while  (found != std::string::npos)
		{
			chunk_size = ft_atoi_hex(this->temp_body.substr(0, found));
			if (chunk_size == 0)
			{
				this->raw_request += this->temp_body.substr(found + 2);
				this->temp_body.clear();
				return (true);
			}
			this->temp_body = this->temp_body.substr(found + 2);
			if (this->temp_body.length() >= chunk_size)
			{
				found = this->temp_body.find("\r\n");
				this->raw_body += this->temp_body.substr(0, found);
				this->temp_body = this->temp_body.substr(found + 2);
			}
			found = this->temp_body.find("\r\n");
		}
	}
	return (false);
}

bool	Request::isValidAuthHeader(Location &loc)
{
	if (loc.getAuthKey() != "")
	{
		char result[200];
		ft_memset(result, 0, 200);
				
		if (this->headers.find(AUTHRIZATION) == this->headers.end())  // auth key 헤더가 아예 안들어왔다.
		{		
			this->client->getResponse().makeErrorReponse(401);
			return (false);
		}
		else
		{
			size_t idx = this->headers[AUTHRIZATION].find_first_of(' ');
			std::string secret = this->headers[AUTHRIZATION].substr(idx + 1);
			base64_decode(secret.c_str(), result, secret.size());
			if (std::string(result) != loc.getAuthKey()) // 키가 맞지 않는다.
			{
				this->client->getResponse().makeErrorReponse(401);
				return (false);
			}
		}
	}
	return (true);
}

bool	Request::isValidMethod(Location &loc)
{
	bool isAllowCheckOkay = false;
	for (std::list<std::string>::iterator iter = loc.getAllowMethods().begin(); iter != loc.getAllowMethods().end(); iter++)
	{
		if (this->method == *iter)
		{
			isAllowCheckOkay = true;
			break ;
		}
	}
	if (isAllowCheckOkay != true) // allow method check 가 안되었다. -> 405
	{
		this->client->getResponse().makeErrorReponse(405);
		return (false);
	}
	return (true);
}
