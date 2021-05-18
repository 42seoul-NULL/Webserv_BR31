#ifndef GET_NEXT_LINE_HPP
# define GET_NEXT_LINE_HPP

# include <unistd.h>
# include <stdlib.h>
# include <limits.h>

#ifndef BUFFER_SIZE
# define BUFFER_SIZE 1024
#endif

int					get_next_line(int fd, char **line);
int					ft_len(const char *src);
int					is_newline(char *str);
int					return_end(int fd, char **bus, char **line, int rb);
int					bu_split(char **line, char *bu);
char				*bu_append(char *bu, char *buff);
char				*ft_strdup(const char *input);
unsigned int		ft_next(char *str, const char *charset, unsigned int s, int is_sep);
char				**ft_split(char *str, const char *charset);


#endif
