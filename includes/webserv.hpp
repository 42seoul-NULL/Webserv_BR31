#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include <stdlib.h>
# include <string>
# include <map>
# include <list>
# include <vector>
# include <unistd.h>
# include <fcntl.h>
# include <iostream>
# include <arpa/inet.h>
# include <sys/socket.h>
# include <ctime>
# include <sys/select.h>
# include <errno.h>
# include <sys/stat.h>
# include <dirent.h>
# include <signal.h>
# include <limits.h>
//error
#include <string.h>
#include <errno.h>


# include "libft.hpp"
# include "enums.hpp"

/*
Config 책임/권한
- .config를 파싱해서 데이터로 보관
- mime type map을 보유
- 응답 status code/설명 map을 보유
- base_64 디코딩 map을 보유
*/

class Nginx; // 싹다 전방위 선언
class Client;
class Request;
class Response;
class Resource;
class Location;
class Server;

///////////////Config////////////////
class Config
{
	private :
		Config();
		Config(const Config &src);
		Config& operator=(const Config& src);
		bool	returnFalseWithMsg(const char *str);
		bool	isReserved(const std::string &src);

		Nginx *nginx;

		std::map<std::string, Server> server_blocks;
		static Config*	instance;
		std::map<std::string, std::string> mime_type;
		std::map<std::string, std::string> status_code;

	public	:
		virtual ~Config();
		static Config* getInstance();
		static const int decodeMimeBase64[256];

		//method
		bool	makeConfig(const char *path);

		//setter
		void	setNginx(Nginx *nginx);

		//getter
		std::map<std::string, Server>& getServerBlocks();
		std::map<std::string, std::string>& getMimeType();
		std::map<std::string, std::string>& getStatusCode();
		Nginx	*getNginx();

		//for test
		void	show();
};

//////////////// Fdmanager ////////////////////
class Fdmanager
{
	protected :
		int				fd;
		e_fd_type			type;

	public	:
		Fdmanager();
		virtual ~Fdmanager();

		void		setFd(int fd);
		int			getFd() const;
		e_fd_type	getType() const;
		void		setType(e_fd_type type);
};

/////////////// Location /////////////////
class Location
{
	private	:
		std::string		uri_key;
		std::string		root;
		std::list<std::string> index;
		std::list<std::string> allow_methods;
		std::map<std::string, std::string> cgi_infos;
		std::map<int, std::string> error_pages;
		int				request_max_body_size;
		std::string		upload_path;
		bool			auto_index;
		std::string		auth_key;
		int				redirect_return;
		std::string		redirect_addr;

	public	:
		Location();
		virtual ~Location(){};
		Location(const Location &src);
		Location& operator=(const Location &src);

		void			setRoot(const std::string &root);
		void			setRequestMaxBodySize(int request_max_body_size);
		void			setUploadPath(const std::string &upload_path);
		void			setAutoIndex(bool auto_index);
		void			setAuthKey(const std::string &auth_key);
		void			setRedirectReturn(int redirect_return);
		void			setRedirectAddr(const std::string &redirect_addr);
		void			setUriKey(const std::string &uri_key);

		const std::string &getRoot();
		std::list<std::string> &getIndex();
		std::list<std::string> &getAllowMethods();
		int getRequestMaxBodySize();
		const std::string &getUploadPath();
		bool	getAutoIndex();
		std::map<std::string, std::string>& getCgiInfos();
		const std::string &getAuthKey();
		int		getRedirectReturn();
		const std::string &getRedirectAddr();
		std::map<int, std::string> &getErrorPages();
		const std::string &getUriKey();

		//for test//
		void	show();
};

/////////////// Server //////////////
class Server	:	public Fdmanager
{
	private	:
		std::string		ip;
		unsigned short	port;
		std::string		server_name;
		std::map<std::string, Location> locations;

	public	:
		Server();
		Server(const Server &src);
		Server& operator=(const Server &src);
		virtual	~Server();

		Location	&getPerfectLocation(std::string &uri);

		void	setPort(unsigned short port);
		void	setIP(const std::string &ip);
		void	setServerName(const std::string &server_name);

		const std::string &getIP() const;
		const std::string &getServerName() const;
		unsigned short	   getPort() const;

		//코딩해야함

		std::map<std::string, Location> &getLocations();
		//for test//
		void	show();
};

////////////////// Resource //////////////////
class Resource	:	public Fdmanager
{
	private :
		std::string &raw_data;
		std::string unlink_path;
		Client		*client;
		e_direction	direction;
		e_nextcall	next_call;
		int	response_error_num;
		size_t		write_index;

		int			pid;
		bool		is_seeked;
	public	:
		Resource(int fd, std::string& raw_data, Client *client, e_direction direction, e_nextcall next_call, int response_error_num);
		virtual ~Resource();

		//method
		e_resource_is_ready_status isReady();
		void		doNext();
		bool		isFdToRawData();
		bool		isRawDataToFd();

		//setter
		void		setResponseErrorNum(int response_error_num);
		void		setPid(int pid);
		void		setUnlinkPath(const std::string& unlink_path);
		void		setWriteIndex(size_t write_index);
		void		setClient(Client *client);

		//getter
		int			getPid();
		const std::string &getUnlinkPath();
		std::string &getRawData();
		Client		*getClient();
		size_t		getWriteIndex();
};

////////////////// Request /////////////////////
class Request
{
	private:
		std::string	raw_request;
		std::string	method;
		std::string	uri;
		std::string	http_version;
		std::map<std::string, std::string> headers;
		std::string	raw_body;

		e_request_status	status;

		Client *client;
		size_t		remain_body_value;

	public:
		//constructor and destroyer
		Request(void);
		virtual		~Request(void){};

		//methods
		void		initRequest(void);
		bool		tryMakeRequest(void);
		//setter
		void				setClient(Client *client);

		//getter
		std::string&		getRawRequest(void);
		const std::string&	getMethod(void) const;
		const std::string&	getUri(void) const;
		const std::string&	getHttpVersion(void) const;
		std::map<std::string, std::string>&	getHeaders(void) const;
		std::string&		getRawBody(void) const;

	private:
		int		 	base64_decode(const char * text, char * dst, int numBytes);
		bool		requestValidCheck(bool isComplete);

		void		makeStartLine(void);
		void		makeRequestHeader(void);

		void		parseMethod(void);
		void		parseUri(void);
		void		parseHttpVersion(void);

		bool		isValidAuthHeader(Location &loc);
		bool		isValidMethod(Location &loc);
		bool		isValidRequestMaxBodySize(Location &loc);
};

/////////////////////////// Response //////////////////////////
class Response
{
	private:
		bool	is_disconnect_immediately;
		Client	*client;

		std::string raw_response;
		std::string	resource_path; // 이전 absolute path
		Location *location;
		std::string cgi_extention;
		bool	is_redirection;
		size_t	write_index;
		std::list<Resource *> resources;

	public :
		Response(void);
		virtual ~Response(void);

		//getter
		bool		getIsDisconnectImmediately();
		std::string&	getRawResponse(void);
		size_t	getWriteIndex();
		std::list<Resource *> &getResources();
		//setter
		void	setLocation(Location *location);
		void	setClient(Client *client);
		void	setResourcePath(const std::string &resource_path);
		void	setCgiExtention(const std::string &cgi_extention);
		void	setIsRedirection(bool is_redirection);
		void	setWriteIndex(size_t index);

		//response_common
		void	initResponse(void);
		void	makeResponse();
		void	makeErrorResponse(int error_num);
		//int		makeCgiResponse();

	private :
		bool	isExist(std::string &path);
		bool	isDirectory(std::string &path);
		void	makeDefaultErrorBody(std::string &body, int error);
		void	makeAutoIndexPage();
		void	setResource(int fd, e_direction direction, e_nextcall nextcall, int error_num = -1);

		int		addFirstLine(int code);
		int		addAllow();
		int		addContentLanguage();
		int		addContentLength(int size);
		int		addContentLocation();
		int		addContentType(const std::string &mime_type);
		int		addDate();
		int		addDate(std::string &target);
		int		addLastModified();
		int		addLocation();
		int		addRetryAfter();
		int		addServer();
		int		addWWWAuthenticate();
		void	addRawErrorBody(int error);
		void	addEmptyline();

		//response_cgi
		void	makeCgiResponse();
		char	**makeCgiEnv();

		//response_get
		void	makeGetResponse();
		void	makeRedirectionResponse();

		//response_put
		// int		makePutResponse();
		// int		makeDirectoryToCreate();
		// int		createPutRequest();
		void		makePutResponse();
		int			mkdirResourcePath();

		//response_head
		void		makeHeadResponse();

		//response_option
		void		makeOptionsResponse();

		//response_delete
		void		makeDeleteResponse();



		// char	**makeEnv();
};

/////////////////// Client ////////////////////
class Client	:	public Fdmanager
{
	private	:
		e_client_status		status;
		Server	*		server;
		Request			request;
		Response		response;
		unsigned long	last_request_ms;

		int				fd_read;
		int				fd_write;

		Client();
	public	:
		//생성자와 소멸자
		Client(Server* server, int fd);
		virtual ~Client();

		//setter
		void		setStatus(e_client_status status);
		void		setLastRequestMs(unsigned long last_request_ms);
		void		setFdRead(int fd_read);
		void		setFdWrite(int fd_write);

		//getter
		Server *	getServer();
		Request		&getRequest();
		Response		&getResponse();
		e_client_status	getStatus();
		unsigned long getLastRequestMs();
		int		getFdRead();
		int		getFdWrite();
};

////////////////// Nginx ////////////////
class Nginx
{
	private :
		fd_set	reads;
		fd_set	writes;
		fd_set	errors;

		int		fd_max;
		std::vector<Fdmanager *> fd_pool;  /// 모든 fd pool (Server, Client, Resource가 담긴다.)
		size_t	connection_time_out;
		typedef std::map<std::string, Server> serverMap;

	public	:
		// 생정자 & 소멸자
		Nginx();
		virtual ~Nginx();

		// public method
		bool	initServers(); // Config 에 적혀있는 정보를 기반으로 Server 인스턴스를 직접 만들어 내서 자신의 fds 에 집어넣는다.
		bool	run();
		void	deleteFromFdPool(Fdmanager * fdmanager);
		void 	insertToFdpool(Fdmanager *fdmanager);
		void	cleanUp();
		void	setConnectionTimeOut(size_t connection_time_out);

	private :
		// initServer()'s
		void	turnOnServerFD(serverMap::iterator server_block);
		void	putServerFdIntoFdPool(serverMap::iterator server_block);
		// run()'s
		bool	isIndexOfReadFdSet(int index, fd_set &reads);
		bool	isIndexOfWriteFdSet(int index, fd_set &writes);
		bool	isIndexOfErrorFdSet(int index, fd_set &errors);
		void	doReadServerFd(int i);
		void	doReadClientFD(int i);
		void	doReadResourceFD(int i);
		void	doWriteClientFD(int i);
		void	doWriteResourceFD(int i);


};

#endif
