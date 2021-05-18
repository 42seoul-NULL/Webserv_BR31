NAME = libft.a

CC = clang++
CFLAGS = -Wall -Wextra -Werror
AR = ar rcs
RM = rm -f

FILES = ft_memset \
		ft_bzero \
		ft_atoi \
		ft_isalpha \
		ft_isdigit \
		ft_isalnum \
		ft_isascii \
		ft_isprint \
		ft_toupper \
		ft_tolower \
		ft_split \
		ft_itoa \
		ft_strmapi \
		ft_putchar_fd \
		ft_putstr_fd \
		ft_putendl_fd \
		ft_putnbr_fd \
		ft_nbr_length \
		get_next_line \
		ft_fd_zero \
		ft_fd_set \
		ft_fd_isset \
		ft_fd_clr \
		ft_htonl \
		ft_htons \
		ft_ntohl \
		ft_ntohs \
		ft_get_time \
		ft_atoi_hex

SRCS_DIR = ./
SRCS = $(addprefix $(SRCS_DIR), $(addsuffix .cpp, $(FILES)))

OBJS_DIR = ./
OBJS = $(addprefix $(OBJS_DIR), $(addsuffix .o, $(FILES)))


.cpp.o: $(SRCS)
	$(CC) $(CFLAGS) -c -o $@ $<

$(NAME): $(OBJS)
	$(AR) $@ $^

all: $(NAME)

clean:
	$(RM) $(OBJS) $(OBJS_B)

fclean: clean
	$(RM) $(NAME)

re: clean all

.PHONY: all clean fclean re