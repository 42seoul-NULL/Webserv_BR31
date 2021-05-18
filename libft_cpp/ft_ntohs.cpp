#include "libft.hpp"

uint16_t ft_ntohs(uint16_t val)
{
	uint16_t temp = 0x1234;
	if (*(unsigned char *)&temp == 0x12)
		return (val);

	return ((val & 0x00FFU) << 8
		| (val & 0xFF00U) >> 8);
}