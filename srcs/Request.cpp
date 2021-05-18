#include "../includes/Request.hpp"

Request::Request(void) : status(0), type(0)
{
	this->raw_request.clear();
	initRequest();
}

Request::Request(const Request& src) : raw_request(src.raw_request), method(src.method), uri(src.uri), http_version(src.http_version), accept_language(src.accept_language), authorization(src.authorization), content_length(src.content_length), content_type(src.content_type), date(src.date), host(src.host), referer(src.referer), transfer_encoding(src.transfer_encoding), user_agent(src.user_agent), status(src.status), type(src.type)
{

}

Request&	Request::operator=(const Request& src)
{
	this->raw_request = src.raw_request;
	this->method = src.method;
	this->uri = src.uri;
	this->http_version = src.http_version;
	this->accept_charsets = src.accept_charsets;
	this->accept_language = src.accept_language;
	this->authorization = src.authorization;
	this->content_length = src.content_length;
	this->content_type = src.content_type;
	this->date = src.date;
	this->host = src.host;
	this->referer = src.referer;
	this->transfer_encoding = src.transfer_encoding;
	this->user_agent = src.user_agent;
	this->status = 0;
	this->type = 0;

	return (*this);
}

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

const std::string&	Request::getAcceptCharsets(void) const
{
	return (this->accept_charsets);
}

const std::string&	Request::getAcceptLanguage(void) const
{
	return (this->accept_language);
}

const std::string&	Request::getAuthorization(void) const  // base64 로 복호화 한 후에    id:pass 로 함
{
	return (this->authorization);
}

const std::string&	Request::getContentLength(void) const
{
	return (this->content_length);
}

const std::string&	Request::getContentType(void) const
{
	return (this->content_type);
}

const std::string&	Request::getDate(void) const
{
	return (this->date);
}

const std::string&	Request::getHost(void) const
{
	return (this->host);
}

const std::string&	Request::getReferer(void) const
{
	return (this->referer);
}

const std::string&	Request::getTransferEncoding(void) const
{
	return (this->transfer_encoding);
}

const std::string&	Request::getUserAgent(void) const
{
	return (this->user_agent);
}

void	Request::setRawRequest(const std::string& raw_request)
{
	this->raw_request = raw_request;
}

void	Request::setAcceptCharsets(const std::string& accept_charsets)
{
	this->accept_charsets = accept_charsets;
}

void	Request::setAcceptLanguage(const std::string& accept_language)
{
	this->accept_language = accept_language;
}

void	Request::setAuthorization(const std::string& authorization)
{
	this->authorization = authorization;
}

void	Request::setContentLength(const std::string& content_length)
{
	this->content_length = content_length;
}

void	Request::setContentType(const std::string& content_type)
{
	this->content_type = content_type;
}

void	Request::setDate(const std::string& date)
{
	this->date = date;
}

void	Request::setHost(const std::string& host)
{
	this->host = host;
}

void	Request::setReferer(const std::string& referer)
{
	this->referer = referer;
}

void	Request::setTransferEncoding(const std::string& transfer_encoding)
{
	this->transfer_encoding = transfer_encoding;
}

void	Request::setUserAgent(const std::string& user_agent)
{
	this->user_agent = user_agent;
}

void	Request::initRequest(void)
{
	this->method.clear();
	this->uri.clear();
	this->http_version.clear();
	
	this->accept_charsets.clear();
	this->accept_language.clear();
	this->authorization.clear();
	this->content_length.clear();
	this->content_type.clear();
	this->date.clear();
	this->host.clear();
	this->referer.clear();
	this->transfer_encoding.clear();
	this->user_agent.clear();

	this->raw_header.clear();
	this->raw_body.clear();
	this->temp_body.clear();

	status = 0;
	type = 0;
}

bool	Request::tryMakeRequest(void)
{
	std::size_t	found = this->raw_request.find("\r\n\r\n");
	int	res = 1;

	if (found != std::string::npos && this->status == 0)
	{
		this->makeStartLine();
		this->makeRequestHeader();
		this->status = 1;
		res = bodyCheck();
		if (res == 0)
		{
			this->raw_request = this->temp_body;
			this->temp_body.clear();
			return (true);
		}
	}
	if (this->status == 1)
	{
		this->makeRequestBody();
		return (isComplete());
	}
	return (false);
}

void	Request::makeStartLine(void)
{
	this->parseMethod();
	this->parseUri();
	this->parseHttpVersion();
}

void	Request::makeRequestHeader(void)
{
	this->raw_header = this->raw_request.substr(this->raw_request.find("\r\n") + 1, this->raw_request.find("\r\n\r\n"));

	this->parseAcceptCharsets();
	this->parseAcceptLanguage();
	this->parseAuthorization();
	this->parseContentLength();
	this->parseContentType();
	this->parseDate();
	this->parseHost();
	this->parseReferer();
	this->parseTransferEncoding();
	this->parseUserAgent();

	this->temp_body = this->raw_request.substr(this->raw_request.find("\r\n\r\n") + 4);
	this->raw_request.clear();
}

void	Request::makeRequestBody(void)
{
	this->temp_body += raw_request;
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

void	Request::parseAcceptCharsets(void)
{
	std::string	key = "Accept-Charset: ";
	std::size_t	found = this->raw_header.find(key);

	if (found != std::string::npos)
	{
		std::size_t	target_pos = found + key.length();

		this->accept_charsets = this->raw_header.substr(target_pos, this->raw_header.find("\r\n", target_pos) - target_pos);
	}
}

void	Request::parseAcceptLanguage(void)
{
	std::string key = "Accept-Language: ";
	std::size_t found = this->raw_header.find(key);

	if (found != std::string::npos)
	{
		std::size_t target_pos = found + key.length();

		this->accept_language = this->raw_header.substr(target_pos, this->raw_header.find("\r\n", target_pos) - target_pos);
	}
}

void	Request::parseAuthorization(void)
{
	std::string key = "Authorization: ";
	std::size_t found = this->raw_header.find(key);

	if (found != std::string::npos)
	{
		std::size_t target_pos = found + key.length();

		this->authorization = this->raw_header.substr(target_pos, this->raw_header.find("\r\n", target_pos) - target_pos);
	}
}

void	Request::parseContentLength(void)
{
	std::string key = "Content-Length: ";
	std::size_t found = this->raw_header.find(key);

	if (found != std::string::npos)
	{
		std::size_t target_pos = found + key.length();

		this->content_length = this->raw_header.substr(target_pos, this->raw_header.find("\r\n", target_pos) - target_pos);
	}
}

void	Request::parseContentType(void)
{
	std::string key = "Content-Type: ";
	std::size_t found = this->raw_header.find(key);

	if (found != std::string::npos)
	{
		std::size_t target_pos = found + key.length();

		this->content_type = this->raw_header.substr(target_pos, this->raw_header.find("\r\n", target_pos) - target_pos);
	}
}

void	Request::parseDate(void)
{
	std::string key = "Date: ";
	std::size_t found = this->raw_header.find(key);

	if (found != std::string::npos)
	{
		std::size_t target_pos = found + key.length();

		this->date = this->raw_header.substr(target_pos, this->raw_header.find("\r\n", target_pos) - target_pos);
	}
}

void	Request::parseHost(void)
{
	std::string key = "Host: ";
	std::size_t found = this->raw_header.find(key);

	if (found != std::string::npos)
	{
		std::size_t target_pos = found + key.length();

		this->host = this->raw_header.substr(target_pos, this->raw_header.find("\r\n", target_pos) - target_pos);
	}
}

void	Request::parseReferer(void)
{
	std::string key = "Referer: ";
	std::size_t found = this->raw_header.find(key);

	if (found != std::string::npos)
	{
		std::size_t target_pos = found + key.length();

		this->referer = this->raw_header.substr(target_pos, this->raw_header.find("\r\n", target_pos) - target_pos);
	}
}

void	Request::parseTransferEncoding(void)
{
	std::string key = "Transfer-Encoding: ";
	std::size_t found = this->raw_header.find(key);

	if (found != std::string::npos)
	{
		std::size_t target_pos = found + key.length();

		this->transfer_encoding = this->raw_header.substr(target_pos, this->raw_header.find("\r\n", target_pos) - target_pos);
	}
}

void	Request::parseUserAgent(void)
{
	std::string key = "User-Agent: ";
	std::size_t found = this->raw_header.find(key);

	if (found != std::string::npos)
	{
		std::size_t target_pos = found + key.length();

		this->user_agent = this->raw_header.substr(target_pos, this->raw_header.find("\r\n", target_pos) - target_pos);
	}
}

bool	Request::bodyCheck(void)
{
	if (this->transfer_encoding == "chunked")
		this->type = 2;
	else if (this->content_length != "")
		this->type = 1;
	return (this->type);
}

bool	Request::isComplete(void)
{
	if (this->type == 1 && this->temp_body.length() >= (std::size_t)ft_atoi(this->content_length))
	{
		this->raw_body += this->temp_body.substr(0, ft_atoi(this->content_length));
		this->raw_request += this->temp_body.substr((std::size_t)ft_atoi(this->content_length) ); // 다음 리퀘스트가 한 번에 붙어서 오면 어떻게 처리해야하는가?
		temp_body.clear();
		return (true);
	}
	else if (this->type == 2)
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

std::string	Request::createRawRequest(void) const
{
	std::string	header = std::string("");

	header += "Method: " + this->method + "\r\n";
	header += "URI: " + this->uri + "\r\n";
	header += "HTTP-Version: " + this->http_version + "\r\n";
	header += "Accept-Charset: " + this->accept_charsets + "\r\n";
	header += "Accept-Language: " + this->accept_language + "\r\n";
	header += "Authorization: " + this->authorization + "\r\n";
	header += "Content-Length: " + this->content_length + "\r\n";
	header += "Content-Type: " + this->content_type + "\r\n";
	header += "Date: " + this->date + "\r\n";
	header += "Host: " + this->host + "\r\n";
	header += "Referer: " + this->referer + "\r\n";
	header += "Transfer-Encoding: " + this->transfer_encoding + "\r\n";
	header += "User-Agent: " + this->user_agent + "\r\n";
	header += "\r\n";

	return (header);
}

void	Request::bodyPrint(void)
{
	std::cout << this->raw_body << std::endl;
}

