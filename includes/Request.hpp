#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <iostream>
# include <vector>
# include "../libft_cpp/libft.hpp"

# define NEEDBODY -1
# define REQUESTCOMPLETE -2

class Request
{
	private:
		std::string	raw_request;

		std::string	method;
		std::string	uri;
		std::string	http_version;

		std::string	accept_charsets;
		std::string	accept_language;
		std::string	authorization;
		std::string	content_length;
		std::string	content_type;
		std::string	date;
		std::string	host;
		std::string	referer;
		std::string	transfer_encoding;
		std::string	user_agent;
		std::string raw_header;
		std::string	raw_body;

		std::string temp_body;
		int	status;
		int	type;

	public:
		Request(void);
		Request(const Request& src);
		virtual ~Request(void){};
		Request& operator=(const Request& src);

		std::string&	getRawRequest(void);
		const std::string&	getMethod(void) const;
		const std::string&	getUri(void) const;
		const std::string&	getHttpVersion(void) const;
		const std::string&	getAcceptCharsets(void) const;
		const std::string&	getAcceptLanguage(void) const;
		const std::string&	getAuthorization(void) const;
		const std::string&	getContentLength(void) const;
		const std::string&	getContentType(void) const;
		const std::string&	getDate(void) const;
		const std::string&	getHost(void) const;
		const std::string&	getReferer(void) const;
		const std::string&	getTransferEncoding(void) const;
		const std::string&	getUserAgent(void) const;

		void	setRawRequest(const std::string& raw_request);
		void	setMethod(const std::string& method);
		void	setUri(const std::string& uri);
		void	setHttpVersion(const std::string& http_version);
		void	setAcceptCharsets(const std::string& accept_charsets);
		void	setAcceptLanguage(const std::string& accept_language);
		void	setAuthorization(const std::string& authorization);
		void	setContentLength(const std::string& content_length);
		void	setContentType(const std::string& content_type);
		void	setDate(const std::string& date);
		void	setHost(const std::string& host);
		void	setReferer(const std::string& referer);
		void	setTransferEncoding(const std::string& transfer_encoding);
		void	setUserAgent(const std::string& user_agent);

		void	initRequest(void);
		bool	tryMakeRequest(void);
	private:
		void	makeStartLine(void);
		void	makeRequestHeader(void);
		void	makeRequestBody(void);

		void	parseMethod(void);
		void	parseUri(void);
		void	parseHttpVersion(void);

		void	parseAcceptCharsets(void);
		void	parseAcceptLanguage(void);
		void	parseAuthorization(void);
		void	parseContentLength(void);
		void	parseContentType(void);
		void	parseDate(void);
		void	parseHost(void);
		void	parseReferer(void);
		void	parseTransferEncoding(void);
		void	parseUserAgent(void);

		bool	bodyCheck(void);
		bool	isComplete(void);

// 테스트용 메소드
	public:
		std::string	createRawRequest(void) const;
		void	bodyPrint(void);
};

#endif
