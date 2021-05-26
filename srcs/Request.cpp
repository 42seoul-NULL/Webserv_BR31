#include "webserv.hpp"


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
	this->status = HEADER_PARING;
	this->remain_body_value = 0;
}

bool			Request::tryMakeRequest(void)
{
	size_t		idx;

	switch (this->status)
	{
	case HEADER_PARING:
	{
		idx = this->raw_request.find("\r\n\r\n");
		if (idx == std::string::npos) // 못찾았을 경우 이대로 종료
			return (requestValidCheck(false));
		else // 찾았을 경우
		{
			makeStartLine();
			makeRequestHeader(); // 헤더까지 모두 만든다.
			//헤더를 만들고 난 직후이므로,
			if ((this->headers.count(TRANSFER_ENCODING) == 1) && (this->headers[TRANSFER_ENCODING] == "chunked")) // content length 필드가 있다 -> 컨텐츠바디리시빙
			{
				// remain_body 를 잴수 있을지 없을지 모른다.
				this->status = CHUNKED_LENGTH_RECEIVING;
				return (tryMakeRequest());
			}
			else if (this->headers.count(CONTENT_LENGTH) == 1)
			{
				this->remain_body_value = ft_atoi(headers[CONTENT_LENGTH]);
				if (this->remain_body_value == 0)
					return (requestValidCheck(true));
				this->status = LENGTH_BODY_RECEIVING;
				return (tryMakeRequest()); // length_body 로 재귀
			}
			else
				return (requestValidCheck(true)); // 바디가 없다는 뜻이니까
		}
		break ;
	}
	case LENGTH_BODY_RECEIVING:
	{
		if (this->remain_body_value <= this->raw_request.size()) // 충분히 잘라낼만큼 있다.
		{
			this->raw_body.append(this->raw_request, 0, this->remain_body_value);
			this->raw_request.erase(0, this->remain_body_value);
			this->remain_body_value = 0;
			return (requestValidCheck(true));
		}
		else  //  충분하지 못하다.
		{
			this->raw_body += this->raw_request;
			this->remain_body_value -= this->raw_request.size();
			this->raw_request.clear();
			return (requestValidCheck(false));
		}
		break ;
	}
	case CHUNKED_LENGTH_RECEIVING:
	{
		// 아직 chunked 의 길이를 알지 못한다.
		idx = this->raw_request.find("\r\n");
		if (idx == std::string::npos)
			return (requestValidCheck(false)); //
		else // 헤더있는줄
		{
			this->remain_body_value = ft_atoi_hex(this->raw_request.substr(0, idx)) + 2; // 실제 데이터 뒤에 있는 \r\n 까지.
			this->raw_request = this->raw_request.substr(idx + 2); // "\r\n" 까지 모조리 없애준다.
			this->status = CHUNKED_BODY_RECEVING;
			return (tryMakeRequest());
		}
		break ;
	}
	case CHUNKED_BODY_RECEVING:
	{
		if (this->remain_body_value <= this->raw_request.size()) // 충분히 잘라낼만큼 있다. // (뒤에있는 \r\n 까지)
		{
			size_t temp_size = this->raw_body.size();
			this->raw_body.append(this->raw_request, 0, this->remain_body_value - 2); // 뒤에있는 \r\n 제외하고 끊어내준다.
			this->raw_request.erase(0, this->remain_body_value); // 뒤에있는 \r\n 까지 끊어준다.

			this->remain_body_value = 0;
			if (this->raw_body.size() == temp_size) // 0 chunked 였다.
				return (requestValidCheck(true));
			this->status = CHUNKED_LENGTH_RECEIVING;
			return (tryMakeRequest());
		}
		break ;
	}
	default:
		break ;
	}
	return (requestValidCheck(false));
}

///////// setter ////////////
void			Request::setClient(Client *client)
{
	this->client = client;
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

std::string&	Request::getRawBody(void) const
{
	return (const_cast<std::string &>(this->raw_body));
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
	std::string raw_header = this->raw_request.substr(0, this->raw_request.find("\r\n\r\n") );

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
	this->raw_request = this->raw_request.substr(this->raw_request.find("\r\n\r\n") + 4);
}

bool	Request::isValidAuthHeader(Location &loc)
{
	if (loc.getAuthKey() != "")
	{
		char result[200];
		ft_memset(result, 0, 200);

		if (this->headers.find(AUTHORIZATION) == this->headers.end())  // auth key 헤더가 아예 안들어왔다.
		{
			return (false);
		}
		else
		{
			size_t idx = this->headers[AUTHORIZATION].find_first_of(' ');
			std::string secret = this->headers[AUTHORIZATION].substr(idx + 1);
			base64_decode(secret.c_str(), result, secret.size());
			if (std::string(result) != loc.getAuthKey()) // 키가 맞지 않는다.
			{
				return (false);
			}
		}
	}
	return (true);
}

bool	Request::isValidRequestMaxBodySize(Location &loc)
{
	if (this->raw_body.size() > (size_t)(loc.getRequestMaxBodySize()))
		return (false);
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
		return (false);
	return (true);
}

bool	Request::requestValidCheck(bool isComplete)
{
	if (isComplete == false)
		return (false);
	else
	{
		Location &loc = this->client->getServer()->getPerfectLocation(this->uri);
		//set response location
		this->client->getResponse().setLocation(&loc);

		//auth check
		if (isValidAuthHeader(loc) == false)
		{
			this->client->setStatus(RESPONSE_MAKING);
			this->client->getResponse().makeErrorResponse(401);
			return (false);
		}
		//allow_method check
		if (isValidMethod(loc) == false)
		{
			this->client->setStatus(RESPONSE_MAKING);
			this->client->getResponse().makeErrorResponse(405);
			return (false);
		}
		//request_max_body_size
		if (isValidRequestMaxBodySize(loc) == false)
		{
			this->client->setStatus(RESPONSE_MAKING);
			this->client->getResponse().makeErrorResponse(413);
			return (false);
		}
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
		//set is_redirection
		if (loc.getRedirectReturn() != -1)
			this->client->getResponse().setIsRedirection(true);

		return (true);
	}
}
