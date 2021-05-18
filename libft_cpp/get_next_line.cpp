#include <string>
#include <unistd.h>

# define OPEN_MAX 10240
# define BUFFER_SIZE 4096

int get_next_line(int fd, std::string &line)
{
	static std::string stored[OPEN_MAX];
	char buf[BUFFER_SIZE + 1];
	int read_size = 1;
	
	std::string temp;
	while ((stored[fd].find('\n') == std::string::npos) && read_size != 0)
	{
		if ((read_size = read(fd, buf, BUFFER_SIZE)) == -1)
			return (-1);
		buf[read_size] = '\0';
		temp = buf;
		stored[fd] += temp;
	}

	std::string::size_type nl_index;
	if ((nl_index = stored[fd].find('\n')) != std::string::npos)
	{
		line = stored[fd].substr(0, nl_index);
		stored[fd] = stored[fd].substr(nl_index + 1);
		return (1);
	}
	line = stored[fd];
	stored[fd].clear();
	return (0);
}
