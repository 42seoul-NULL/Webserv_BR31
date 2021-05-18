#include "libft.hpp"

void	ft_putstr_fd(const std::string &str, int fd)
{
	if (fd < 0)
		return ;
	write(fd, str.c_str(), str.length());
}
