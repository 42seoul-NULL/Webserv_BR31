#include "libft.hpp"

int ft_atoi_hex(const std::string &str)
{
	int result = 0;

	for (std::string::const_iterator iter = str.begin(); iter != str.end(); iter++)
	{
		if ( *iter >= '0' && *iter <= '9' )
		{
			result = result * 16 + (*iter - '0');
		}
		else if ( *iter >= 'a' && *iter <= 'f')
		{
			result = result * 16 + (*iter - 'a' + 10);
		}
		else if ( *iter >= 'A' && *iter <= 'F')
		{
			result = result * 16 + (*iter - 'A' + 10);
		}
		else
			break ;
	}
	return (result);
}