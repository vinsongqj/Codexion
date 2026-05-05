NAME = codexion
CC = cc 
CFLAGS = -Wall -Wextra -Werror -pthread
SRCS = src/main.c src/actions.c src/scheduler.c src/threads.c src/utils.c
OBJS = $(SRCS:.c=.o)
HEADER = codexion.h 

GREEN = \033[0;32m
RED = \033[0;31m
RESET = \033[0m

all: $(NAME)

$(NAME): $(OBJS)
		@$(CC) $(CFLAGS) $(OBJS) -o $(NAME)
		@echo "$(GREEN)Codexion compiled successfully$(RESET)"

src/%.o: %.c	$(HEADER)
			@$(CC) $(CFLAGS) -c $< -o $@

clean:
		@rm -f $(OBJS)
		@echo "$(RED)Objects removed$(RESET)"
	
fclean:	clean
		@rm -f $(NAME)
		@echo "$(RED)Build files cleaned$(RESET)"

re: fclean all

.PHONY: all clean fclean re