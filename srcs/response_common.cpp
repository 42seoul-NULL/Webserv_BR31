#include "webserv.hpp"

void	Response::initResponse(void)
{
	this->raw_response.clear();
	this->resource_path.clear();
	this->is_redirection = false;
	this->cgi_extention.clear();
	this->location = NULL;
}

void	Response::makeResponse()
{
	//	여기까지 왔다는 것은 method allow check 되어있고, authcheck 되어있고,
    if (cgi_extention != "")
		return (makeCgiResponse());
	if (is_redirection)
		return (makeRedirectionResponse());

	if (this->client->getRequest().getMethod() == "GET")
		makeGetResponse();
	else if (this->client->getRequest().getMethod() == "PUT")
		makePutResponse();
}


void	Response::makeErrorResponse(int error)
{
	this->raw_response.clear();

	addFirstLine(error);
	addAllow();
	addDate();
	addServer();
	addContentType(".html");

	if (error == 401)
		addWWWAuthenticate();
	switch (this->client->getStatus())
	{
	case RESPONSE_MAKING:
	{
		if (this->location->getErrorPages().count(error) == 0) // 디폴트 에러 페이지 없음.
			addRawErrorBody(error);
		else // 디폴트 에러 페이지 존재함.
		{
			int fd = open(this->location->getErrorPages()[error].c_str(), O_RDONLY);
			if (fd == -1) // 실패 -> 디폴트페이지가 아니라 자체적으로 만들어내는 페이지로 리턴
				addRawErrorBody(error);
			else
			{
				//컨텐츠 랭스 적고 등등 해줘야함
				struct stat sb;

				fstat(fd, &sb);
				addContentLength(sb.st_size);
				this->raw_response += "\r\n";
				setResource(fd, FD_TO_RAW_DATA, MAKE_ERROR_RESPONSE, error); // 이녀석이 알아서 자기 상태까지 변환해줌.
				return ;
			}
		}
		break ;
	}
	default:
		break;
	}
}

bool	Response::isExist(std::string &path)
{
	struct stat buffer;
	return ( stat(path.c_str(), &buffer) == 0 );
}

bool	Response::isDirectory(std::string &path)
{
	struct  stat buffer;
	if ( stat(path.c_str(), &buffer) != 0 ) // 안열렸다
		return (false);
	return (S_ISDIR(buffer.st_mode));
}

void	Response::setResource(int fd, e_direction direction, e_nextcall nextcall, int error_num)
{
	Resource *res;
	if (direction == RAW_DATA_TO_FD) // request 의 raw_body 에서 fd 로 써야한다
	{
		res = new Resource(
										fd,
										this->client->getRequest().getRawBody(),
										this->client,
										RAW_DATA_TO_FD,
										nextcall,
										error_num
									);
	}
	else		// fd 에서 response 의 raw_response 로 써야한다. FD_TO_RAW_DATA
	{
		res = new Resource(
										fd,
										this->raw_response,
										this->client,
										FD_TO_RAW_DATA,
										nextcall,
										error_num
									);
	}
	this->client->setPulishedResource(res);
	Config::getInstance()->getNginx()->insertToFdpool(res);

}

/////////////// header make 관련 /////////////////
int		Response::addAllow()
{
	int ret = 405;

	this->raw_response += "Allow:";
	for (std::list<std::string>::iterator iter = this->location->getAllowMethods().begin(); iter != this->location->getAllowMethods().end(); iter++)
	{
		this->raw_response += ' ';
		this->raw_response += (*iter);
		if (this->client->getRequest().getMethod() == *iter)
			ret = 200;
	}
	this->raw_response += "\r\n";
	return (ret);
}

void	Response::makeDefaultErrorBody(std::string &body, int error)
{
	body += "<!DOCTYPE html>\n";
	body += "<html>\n";
	body += "<head>\n";
	body += "</head>\n";
	body += "<body>\n";
	body += "<h1>";
	body += ft_itoa(error);
	body += "<h1>\n";
	body += "</body>\n";
	body += "</html>";
}

///// public method ///////

int		Response::addFirstLine(int code)
{
	std::string temp = ft_itoa(code);
	this->raw_response += ("HTTP/1.1 " + temp + " " + Config::getInstance()->getStatusCode()[temp] + "\r\n");
	return (code);
}

int		Response::addContentLanguage()
{
	this->raw_response += "Content-Language: ko-kr\r\n";
	return (200);
}

int		Response::addContentLength(int size)
{
	this->raw_response += "Content-Length: " + ft_itoa(size) + "\r\n";
	return (200);
}


int		Response::addContentLocation()
{
	// 서버에 대한 리소스 접근의 절대적 경로.
	// 추가적인 URI 파싱을 진행 후 서버에 대한 정리를 한 후 해야할 듯
	// 서버 내의 파일 형태와 구조에 대해 생각 해 보아야 함.
	// 지금 작성한 내용은 루트에 대해 해당하는 내용임

	// 반환된 데이터에 대한 위치 //   처리하는 location 이 리다이렉션이 있을 경우 리다이렉션을 처리해서 리다이렉션 주소를 여기에 찍어줌. 예시) content_location: https://www.naver.com\r\n
	this->raw_response += "Content-Location: " + this->resource_path + "\r\n";
	return (200);
}

int		Response::addContentType(const std::string &extension)
{
	if (Config::getInstance()->getMimeType().count(extension) == 0) // 없다
		this->raw_response += "Content-Type: " + Config::getInstance()->getMimeType()[".bin"] + "\r\n";
	else // 있다.
		this->raw_response += "Content-Type: " + Config::getInstance()->getMimeType()[extension] + "\r\n";
	return (200);
}

int		Response::addDate()
{
	// Date 함수 살펴 본 후 작성하자.
	// 메시지가 만들어진 날짜, 객체 생성 시의 시간? 보내기 전 Raw화 하기전의 시간?
	// juyang 의 유산
	time_t t;
	char buffer[4096];
	struct tm* timeinfo;

	t = time(NULL);
	timeinfo = localtime(&t);
	strftime(buffer, 4096, "%a, %d %b %Y %H:%M:%S GMT", timeinfo);
	this->raw_response += "Date: " + std::string(buffer) + "\r\n";
	return (200);
}

int		Response::addLastModified()
{
	struct stat	sb;
	struct tm*	timeinfo;
	char buffer[4096];

	if (stat(this->resource_path.c_str(), &sb) < 0)
		return (500);
	timeinfo = localtime(&sb.st_mtime);
	strftime(buffer, 4096, "%a, %d %b %Y %H:%M:%S GMT", timeinfo); // 연도 잘 안나옴
	this->raw_response += "Last-Modified: " + std::string(buffer) + "\r\n";
	return (200);
}

int		Response::addLocation()
{
	//redirection
	this->raw_response += "Location: " + this->location->getRedirectAddr() + "\r\n";
	return (200);
}

int		Response::addRetryAfter()
{
	this->raw_response += "Retry-After: 120\r\n";
	return (200);
}

int		Response::addServer()
{
	this->raw_response += "Server: ft_nginx_too_many_drivers_without_license\r\n";
	return (200);
}

int		Response::addWWWAuthenticate()
{
	// HTTPS를 단순하게나마 따라하기 위해 작성하는 듯 함.
	// 첫 Client의 Request를 401 Unauthorized 로 인증 요청 후 재전송 된 Authorization 필드를 이용해 작성
	// config의 auth_basic 관련 해서 회의 필요.
	// 우리는 거대한 서비스 작성이 아니니 basic type 으로 고정
	this->is_disconnect_immediately = true;
	this->raw_response += "WWW-Authenticate: Basic realm=\"Give me ID:PASS encoded base64\"";
	return (200);
}

void	Response::addRawErrorBody(int error)
{
	std::string temp;
	makeDefaultErrorBody(temp, error);
	addContentLength(temp.size());
	this->raw_response += "\r\n";
	this->raw_response += temp;
	this->client->setStatus(RESPONSE_MAKE_DONE);
}
