#include "libft.hpp"

int FT_FD_ISSET(int fd, fd_set *p)
{
	int mask = 1 << (fd % (sizeof(__int32_t) * 8));
	return (p->fds_bits[fd / (sizeof(__int32_t) * 8)] & mask);
}
