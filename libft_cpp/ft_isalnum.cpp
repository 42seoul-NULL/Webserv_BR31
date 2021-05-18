#include "libft.hpp"

int	ft_isalnum(int c)
{
	return (ft_isalpha(c) || ft_isdigit(c));
}