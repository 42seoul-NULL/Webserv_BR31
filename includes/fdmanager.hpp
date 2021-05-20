#ifndef FDMANAGER_HPP
# define FDMANAGER_HPP

# include "enums.hpp"

class Fdmanager
{
	protected :
		int				fd;
		e_fd_type			type;

	public	:
		Fdmanager();
		virtual ~Fdmanager();

		void		setFd(int fd);
		int			getFd() const;
		e_fd_type	getType() const;
		void		setType(e_fd_type type);
};

#endif