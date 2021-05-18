#include "libft.hpp"

uint32_t ft_htonl(uint32_t val)
{
	uint32_t temp = 0x12345678;
	if (*(unsigned char *)&temp == 0x12)
		return (val);

	return ((val & 0x000000FFU) << 24
		| (val & 0x0000FF00U) << 8
		| (val & 0x00FF0000U) >> 8
		| (val & 0xFF000000U) >> 24);
}