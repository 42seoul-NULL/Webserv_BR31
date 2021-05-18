#include "../includes/parser.hpp"
#include "../includes/Response.hpp"

Response::Response(void)
{
	initResponse();
}

Response::Response(const Response& src) : raw_response(src.raw_response)
{

}

Response::~Response(void)
{

}

Response&	Response::operator=(const Response& src)
{
	this->raw_response = src.raw_response;
	return (*this);
}

int		 	Response::base64_decode(const char * text, char * dst, int numBytes)
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

void	Response::initResponse(void)
{
	this->raw_response.clear();
}

int		Response::getLastResponse()
{
	return (this->last_reponse);
}

int		Response::makeAllow(const Request& request, Location& location)
{
	int ret = 405;

	this->raw_response += "Allow:";
	for (std::list<std::string>::iterator iter = location.getAllowMethods().begin(); iter != location.getAllowMethods().end(); iter++)
	{
		this->raw_response += ' ';
		this->raw_response += (*iter);
		if (request.getMethod() == *iter)
			ret = 200;
	}
	this->raw_response += "\r\n";
	return (ret);
}

void	Response::makeDefaultBody(std::string &body, int error)
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

int		Response::checkAuth(const Request &request, Location &location)
{
	if (location.getAuthKey() == "")
		return (200);
	char result[200];
	ft_memset(result, 0, 200);
	size_t idx = request.getAuthorization().find_first_of(' ');
	std::string secret = request.getAuthorization().substr(idx + 1);
	base64_decode(secret.c_str(), result, secret.size());
	if (std::string(result) == location.getAuthKey())
		return (200);
	return (401);
}

void	Response::makeErrorReponse(const Request &request, Location &location, int error)
{
	initResponse();

	//디폴트 에러 페이지 존재할경우 이것으로 처리.

	std::string body;
	
	if (location.getErrorPages().find(error) == location.getErrorPages().end())
		makeDefaultBody(body, error);
	else
	{
		size_t buffer_size = 1000;
		int fd;
		char buf[buffer_size + 1];
		int rb;

		fd = open(location.getErrorPages()[error].c_str(), O_RDONLY);
		if (fd < 0)
			makeDefaultBody(body, error);
		else
		{
			while ( (rb = read(fd, buf, buffer_size)) > 0 )
			{
				buf[rb] = 0;
				body += std::string(buf);
			}
			if (rb < 0)
			{
				body.clear();
				makeDefaultBody(body, error);
			}
			close (fd);
		}
	}

	makeFirstLine(error);
	this->raw_response += "Allow:";
	for (std::list<std::string>::iterator iter = location.getAllowMethods().begin(); iter != location.getAllowMethods().end(); iter++)
	{
		raw_response += ' ';
		raw_response += *iter;
	}
	raw_response += "\r\n";
	makeDate(request);
	this->raw_response += "Content-Type: " + Config::getInstance()->getMimeType()[".html"] + "\r\n";
	this->raw_response += "Content-Length: " + ft_itoa(body.size()) + "\r\n";
	if (error == 401)
		makeWWWAuthenticate();
	this->raw_response += "\r\n";
	this->raw_response += body;
	
	return ;
}

int		Response::makeContentLanguage()
{
	this->raw_response += "Content-Language: ko-kr\r\n";
	return (200);
}

int		Response::makeContentLength(int size)
{
	this->raw_response += "Content-Length: " + ft_itoa(size) + "\r\n";
	return (200);
}

int		Response::makeContentLocation(const Request& request, Location &location)
{
	// 서버에 대한 리소스 접근의 절대적 경로.
	// 추가적인 URI 파싱을 진행 후 서버에 대한 정리를 한 후 해야할 듯
	// 서버 내의 파일 형태와 구조에 대해 생각 해 보아야 함.
	// 지금 작성한 내용은 루트에 대해 해당하는 내용임

	// 반환된 데이터에 대한 위치 //   처리하는 location 이 리다이렉션이 있을 경우 리다이렉션을 처리해서 리다이렉션 주소를 여기에 찍어줌. 예시) content_location: https://www.naver.com\r\n
	if (request.getMethod() == "GET" || request.getMethod() == "HEAD")
	{
		std::string absol_path(location.getRoot());
		absol_path.erase(--(absol_path.end()));
		absol_path += request.getUri();

		this->raw_response += "Content-Location: " + absol_path + "\r\n";		
	}
	return (200);
}

int		Response::makeContentType(const Request& request, const std::string &mime_type)
{
	if (request.getMethod() == "GET" || request.getMethod() == "HEAD")
	{
		// GET // URI    /honlee/asdf.jpg    /naver.com/hello
		// GET / http/1.1    /docs/
		this->raw_response += "Content-Type: " + mime_type + "\r\n";
	}
	return (200);
}

int		Response::makeDate(const Request& request)
{
	// Date 함수 살펴 본 후 작성하자.
	// 메시지가 만들어진 날짜, 객체 생성 시의 시간? 보내기 전 Raw화 하기전의 시간?
	// juyang 의 유산
	time_t t;
	char buffer[4096];
	struct tm* timeinfo;
	(void)request;

	t = time(NULL);
	timeinfo = localtime(&t);
	strftime(buffer, 4096, "%a, %d %b %Y %H:%M:%S GMT", timeinfo);
	this->raw_response += "Date: " + std::string(buffer) + "\r\n";
	return (200);
}

int		Response::makeLastModified(const Request& request, Location &location)
{
	struct stat	sb;
	struct tm*	timeinfo;
	char buffer[4096];
	(void)request;

	std::string absol_path(location.getRoot());
	absol_path.erase(--(absol_path.end()));
	absol_path += request.getUri();

	if (stat(absol_path.c_str(), &sb) < 0)
		return (500);
	timeinfo = localtime(&sb.st_mtime);
	strftime(buffer, 4096, "%a, %d %b %Y %H:%M:%S GMT", timeinfo); // 연도 잘 안나옴
	this->raw_response += "Last-Modified: " + std::string(buffer) + "\r\n";
	return (200);
}

int		Response::makeLocation(Location &location)
{
	//redirection
	this->raw_response += "Location: " + location.getRedirectAddr() + "\r\n";
	return (200);
}

int		Response::makeRetryAfter()
{
	this->raw_response += "Retry-After: 120\r\n"; 
	return (200);
}

int		Response::makeServer()
{
	this->raw_response += "Server: ft_nginx_too_many_drivers_without_license\r\n";
	return (200);
}

int		Response::makeWWWAuthenticate()
{
	// HTTPS를 단순하게나마 따라하기 위해 작성하는 듯 함.
	// 첫 Client의 Request를 401 Unauthorized 로 인증 요청 후 재전송 된 Authorization 필드를 이용해 작성
	// config의 auth_basic 관련 해서 회의 필요.
	// 우리는 거대한 서비스 작성이 아니니 basic type 으로 고정

	this->raw_response += "WWW-Authenticate: Basic realm=\"Give me ID:PASS encoded base64\"";
	return (200);
}

int		Response::makeFirstLine(int code)
{
	std::string temp = ft_itoa(code);
	this->raw_response += ("HTTP/1.1 " + temp + " " + Config::getInstance()->getStatusCode()[temp] + "\r\n");
	return (code);
}

int		Response::makeAutoIndexPage(const Request& request, const std::string &path)
{
	initResponse();

	std::cout << "makeautoindexpage" << std::endl;

	std::string body;
	std::string pre_addr = "http://" + request.getHost() + "/";

	body += "<!DOCTYPE html>";
	body += "<html>";
	body += "<head>";
	body += "</head>";
	body += "<body>";
	body += "<h1> AutoIndex : "+ request.getUri() +"</h1>";

	DIR *dir = NULL;
	struct dirent *file = NULL;
	if ( (dir = opendir(path.c_str())) == NULL )
		return (500);
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

	makeFirstLine(200);
	makeDate(request);
	this->raw_response += "Content-Type: " + Config::getInstance()->getMimeType()[".html"] + "\r\n";
	this->raw_response += "Content-Length: " + ft_itoa(body.size()) + "\r\n";
	this->raw_response += "\r\n";
	this->raw_response += body;
	return (200);
}

int		Response::makeBody(const Request& request, Location &location)
{
	if (request.getMethod() != "GET")
		return (200);

	//여기서 만들기 직전에 makeContentType 호출
	int fd;
	struct stat	sb;
	size_t buffer_size = 1001;
	char buf[buffer_size + 1];
	int rb;
	size_t idx;

	std::string absol_path(location.getRoot());
	absol_path.erase(--(absol_path.end()));
	absol_path += request.getUri();

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

	while ( (rb = read(fd, buf, buffer_size)) > 0 )
	{
		buf[rb] = 0;
		this->raw_response += buf;
	}
	if (rb < 0)
	{
		close(fd);
		return (500);
	}
	close(fd);
	return (200);
}

int		Response::makeRedirectionResponse(const Request& request, Location& location)
{
	int ret;
	initResponse();
	if (
			(ret = checkAuth(request, location)) != 200 ||
			(ret = makeFirstLine(location.getRedirectReturn())) != location.getRedirectReturn() ||
			(ret = makeDate(request)) != 200 ||
			(ret = makeServer()) != 200 ||
			(ret = makeLocation(location)) != 200
		)
	{
		makeErrorReponse(request, location, ret);
		return (this->last_reponse = ret);
	}
	return (this->last_reponse = location.getRedirectReturn());
}

int		Response::makeResponse(const Request& request, Location &location)
{
	int ret;

	if (location.getRedirectReturn() != -1)
		return (this->last_reponse = makeRedirectionResponse(request, location));

	if (request.getMethod() == "GET" || request.getMethod() == "HEAD")
	{
		if (
				(ret = checkAuth(request, location)) != 200 ||
				(ret = makeFirstLine(200)) != 200 ||
				(ret = makeAllow(request, location)) != 200 ||
				(ret = makeContentLanguage()) != 200 ||
				(ret = makeContentLocation(request, location)) != 200 ||
				(ret = makeDate(request)) != 200 ||
				(ret = makeRetryAfter()) != 200 ||
				(ret = makeServer()) != 200 ||
				(ret = makeWWWAuthenticate()) != 200 ||
				(ret = makeBody(request, location)) != 200
			)
		{
			makeErrorReponse(request, location, ret);
			return (this->last_reponse = ret);
		}
		return (this->last_reponse = 200);
	}
	return (this->last_reponse = 200);
}

const std::string&	Response::getRawResponse(void)
{
	return (this->raw_response);
}