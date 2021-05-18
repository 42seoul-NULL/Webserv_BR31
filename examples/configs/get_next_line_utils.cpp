#include "get_next_line.hpp"

int					ft_len(const char *str)
{
	int				ret;

	ret = 0;
	while (str[ret] != 0)
		ret++;
	return (ret);
}

int					is_newline(char *str)
{
	int				idx;

	idx = 0;
	while (str[idx] != 0)
	{
		if (str[idx] == '\n')
			return (idx);
		idx++;
	}
	return (-1);
}

char				*ft_strdup(const char *input)
{
	char			*ret;
	int				len;
	int				idx;

	len = ft_len(input);
	if (!(ret = (char *)malloc(len + 1)))
		return (NULL);
	idx = -1;
	while (input[++idx] != 0)
		ret[idx] = input[idx];
	ret[idx] = 0;
	return (ret);
}
