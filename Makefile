NAME = webserv

SRCNAME	=	client.cpp\
			config.cpp\
			fdmanager.cpp\
			main.cpp\
			nginx.cpp\
			request.cpp\
			resource.cpp\
			response_common.cpp\
			response_get.cpp\
			response_head.cpp\
			response_put.cpp\
			response_options.cpp\
			response_delete.cpp\
			response_redirection.cpp\
			response_cgi.cpp\
			response.cpp\
			server.cpp\
			fdsets.cpp

SRCDIR = ./srcs/
SRCS = $(addprefix $(SRCDIR), $(SRCNAME))

INC_DIR = ./includes/
INCLUDE = -I$(INC_DIR) -I$(LIB_DIR)

LIB_DIR = ./libft_cpp/
LIB_NAME = libft.a

CC = clang++

#CFLAGS = -Wall -Wextra -Werror -fsanitize=address -g3 -std=c++98
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
	lldb webserv -- tests/sample/sample.config

$(NAME) : $(LIB_DIR)$(LIB_NAME) $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -L$(LIB_DIR) -lft $(INCLUDE) -o $(NAME)

$(LIB_DIR)$(LIB_NAME) :
	make -C $(LIB_DIR) all

###### 여기부터 테스터 관련 설정 ####
TESTS_DIR = ./tests/
TEST1 = test1_tester
TEST1_PORT = 8080

TEST2 = sample

run : all
	./$(NAME) $(TESTS_DIR)$(TEST1)/$(TEST1).config
test1 : all
	./$(NAME) $(TESTS_DIR)$(TEST1)/$(TEST1).config &> testlog &
	-./tests/tester_bin/tester http://localhost:$(TEST1_PORT)
	killall -2 $(NAME)

test2 : all
	./$(NAME) $(TESTS_DIR)$(TEST2)/$(TEST2).config

##############################

# 시즈 명령어

# siege -R <(echo connection = keep-alive) -c10 http://localhost:8080


.PHONY : all clean fclean re dbg test1 test2
