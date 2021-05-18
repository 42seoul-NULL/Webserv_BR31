#ifndef LIBFT_HPP
# define LIBFT_HPP

# include <stdlib.h>
# include <fcntl.h>
# include <unistd.h>
# include <stddef.h>
# include <sys/time.h>
# include <string>
# include <vector>

void				*ft_memset(void *s, int c, size_t n);
void				ft_bzero(void *s, size_t n);
int					ft_atoi(const std::string& str);
int					ft_isalpha(int c);
int					ft_isdigit(int c);
int					ft_isalnum(int c);
int					ft_isascii(int c);
int					ft_isprint(int c);
int					ft_toupper(int c);
int					ft_tolower(int c);

bool				ft_split(const std::string &target, const std::string& sep, std::vector<std::string> &saver);
std::string			ft_itoa(int n);
std::string			ft_strmapi(const std::string& s, char (*f)(unsigned int, char));
void				ft_putchar_fd(char c, int fd);
void				ft_putstr_fd(const std::string &str, int fd);
void				ft_putendl_fd(const std::string &str, int fd);
void				ft_putnbr_fd(int n, int fd);

int					ft_nbr_length(int n);
int					get_next_line(int fd, std::string &line);

void				FT_FD_ZERO(fd_set *p);
void				FT_FD_SET(int fd, fd_set *p);
int					FT_FD_ISSET(int fd, fd_set *p);
void				FT_FD_CLR(int fd, fd_set *p);

uint32_t			ft_htonl(uint32_t val);
uint16_t			ft_htons(uint16_t val);
uint32_t			ft_ntohl(uint32_t val);
uint16_t			ft_ntohs(uint16_t val);

unsigned long		ft_get_time(void);
int					ft_atoi_hex(const std::string &str);

#endif
