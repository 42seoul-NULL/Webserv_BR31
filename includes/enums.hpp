#ifndef ENUMS_HPP
# define ENUMS_HPP

# define BUFFER_SIZE	65536
# define CLIENT_QUE_SIZE 5

# define NEEDBODY -1
# define REQUESTCOMPLETE -2

# define DATE "Date"
# define ALLOW "Allow"
# define CONTENT_LANGUAGE "Content_Language"
# define CONTENT_LOCATION "Content-Location"
# define CONTENT_TYPE "Content-Type"
# define AUTHORIZATION "Authorization"
# define WWW_AUTHENTICATE "WWW-Authenticate"
# define HOST "Host"
# define ACCEPT_CHARSET "Accept-Charset"
# define ACCEPT_LANGUAGE "Accept-Language"
# define REFERER "Referer"
# define USER_AGENT "User-Agent"
# define SERVER "Server"
# define TRANSFER_ENCODING "Transfer-Encoding"
# define LOCATION "Location"
# define LAST_MODIFIED "Last-Modified"
# define RETRY_AFTER "Retry-After"

typedef enum			t_client_status
{
	REQUEST_RECEIVING,
	RESPONSE_MAKING,
	FILE_READING,
	FILE_READ_DONE,
	FILE_WRITING,
	FILE_WRITE_DONE,
	RESPONSE_MAKE_DONE
}						e_client_status;

typedef enum			t_request_try_make_request_return
{
	READY_TO_MAKE_RESPONSE,
	WAITING_REQUEST_MSG,
	I_MAKE_ERROR_RESPONSE
}						e_request_try_make_request_return;

typedef enum			t_body_length_info
{
	NOTHING,
	CONTENT_LENGTH,
	CHUNKED
}						e_body_length_info;

typedef enum			t_fd_type
{
	FD_BASE,
	FD_SERVER,
	FD_CLIENT,
	FD_RESOURCE
}						e_fd_type;

typedef enum			t_direction
{
	RAW_DATA_TO_FD,
	FD_TO_RAW_DATA
}						e_direction;

typedef enum			t_nextcall
{
	MAKE_ERROR_RESPONSE,
	MAKE_RESPONSE
}						e_nextcall;

typedef	enum			t_resource_is_ready_status
{
	READY,
	CGI_CRASH,	
	NOT_YET
}						e_resource_is_ready_status;

#endif