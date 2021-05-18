#include "libft.hpp"

void FT_FD_ZERO(fd_set *p)
{
	for (int i = 0; i < 32; ++i)
		p->fds_bits[i] = 0;
}
