SRCNAME	=		\
					main.cpp\
					parser.cpp\
					Request.cpp\
					Response.cpp\
					nginx.cpp

SRCDIR		=		./srcs/

SRCS		=		${addprefix ${SRCDIR}, ${SRCNAME}}

INCDIR		=		./includes/

NAME		=		webserv

LIB_NAME	=		libft.a

CC			=		clang++

CF			=		-Wall -Wextra -Werror -std=c++98 -I ${INCDIR} ${SRCS}
DCF			=		-g ${SRCS}

${NAME}     :
					make all -C "./libft_cpp"
					cp libft_cpp/${LIB_NAME} ${LIB_NAME}
					${CC} ${CF} ${LIB_NAME} -o ${NAME} 

dbg		:
					${CC} ${DCF} ${LIB_NAME} -o ${NAME}
					lldb webserv -- configs/test.conf

test		:
					${CC} ${DCF} ${LIB_NAME} -o ${NAME}
					./webserv configs/test.conf

fclean		:
					make fclean -C "./libft_cpp"
					rm -rf webserv.dSYM
					rm -rf ${NAME}
					rm -rf ${LIB_NAME}

re			:		fclean all

all         :      	${NAME}

.PHONY		:		fclean re all test ${NAME}
