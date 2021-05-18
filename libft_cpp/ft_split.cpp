#include "libft.hpp"

bool				ft_split(const std::string &target, const std::string& sep, std::vector<std::string> &saver)
{
	std::string temp;

	for (std::string::const_iterator iter = target.begin(); iter != target.end(); iter++)
	{
		if (sep.find(*iter) == std::string::npos )
			temp += *iter;
		else
		{
			if (temp != "")
			{
				saver.push_back(temp);
				temp.clear();
			}
		}
	}
	if (temp != "")
		saver.push_back(temp);
	return (true);
}
