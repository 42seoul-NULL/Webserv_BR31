#include "libft.hpp"

std::string ft_itoa(int n)
{
	int		len;
	int		i;
	int		temp;
	unsigned char	buf[12];

	len = ft_nbr_length(n);
	if (n == 0)
		buf[0] = '0';
	else
	{
		i = len - 1;
		while (n != 0)
		{
			temp = n % 10;
			buf[i--] = temp < 0 ? (-temp + '0') : (temp + '0');
			n /= 10;
		}
		if (i == 0)
			buf[0] = '-';
	}
	buf[len] = 0;
	return (std::string((const char *)buf));
}
