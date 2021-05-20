NAME = webserv

SRCNAME	=	main.cpp\
			config.cpp\
			request.cpp\
			response.cpp\
			response_common.cpp\
			response_get.cpp\
			response_redirection.cpp\
			fdmanager.cpp\
			nginx.cpp

SRCDIR = ./srcs/
SRCS = $(addprefix $(SRCDIR), $(SRCNAME))

INC_DIR = ./includes/
INCLUDE = -I$(INC_DIR) -I$(LIB_DIR)

LIB_DIR = ./libft_cpp/
LIB_NAME = libft.a

CC = clang++
CFLAGS = -Wall -Wextra -Werror -std=c++98
DCFLAGS = -g $(SRCS)

all : $(NAME)

clean :
	make -C $(LIB_DIR) clean

fclean : clean
	make -C $(LIB_DIR) fclean
	rm -rf webserv.dSYM
	rm -rf $(NAME) testlog

re : fclean all

dbg : $(SRCS)
	$(CC) $(DCFLAGS) -L$(LIB_DIR) -lft $(INCLUDE) -o $(NAME)
	lldb webserv -- configs/test.conf

$(NAME) : $(LIB_DIR)$(LIB_NAME) $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -L$(LIB_DIR) -lft $(INCLUDE) -o $(NAME)

$(LIB_DIR)$(LIB_NAME) :
	make -C $(LIB_DIR) all

###### 여기부터 테스터 관련 설정 ####
TESTS_DIR = ./tests/
TEST1 = test1_tester
TEST1_PORT = 8080

run : all
	./$(NAME) $(TESTS_DIR)$(TEST1)/$(TEST1).config
test1 : all
	./$(NAME) $(TESTS_DIR)$(TEST1)/$(TEST1).config > testlog &
	-./tests/tester_bin/tester http://localhost:$(TEST1_PORT)
	killall $(NAME)

##############################

.PHONY : all clean fclean re dbg test1
