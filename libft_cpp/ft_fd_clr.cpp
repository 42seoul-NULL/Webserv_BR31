#include "libft.hpp"

void FT_FD_CLR(int fd, fd_set *p)
{
	int mask = ~(1 << (fd % (sizeof(__int32_t) * 8)));
    p->fds_bits[fd / (sizeof(__int32_t) * 8)] &= mask;
}
