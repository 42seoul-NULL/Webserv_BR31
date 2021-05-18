#include "libft.hpp"

void	ft_bzero(void *s, size_t n)
{
	unsigned char *temp;

	temp = (unsigned char *)s;
	while (n--)
		*(temp++) = 0;
}
