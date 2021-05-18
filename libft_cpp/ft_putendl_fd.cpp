#include "libft.hpp"

void	ft_putendl_fd(const std::string &str, int fd)
{
	if (fd < 0)
		return ;
	write(fd, str.c_str(), str.length());
	write(fd, "\n", 1);
}
