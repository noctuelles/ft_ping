SRCS_DIR=srcs
INCLUDES_DIR=includes
OBJS_DIR=objs

SRCS=main.c
OBJS=$(addprefix $(OBJS_DIR)/, $(SRCS:.c=.o))
INCLUDES=$(addprefix -I, $(INCLUDES_DIR))

NAME=ft_ping

CC=cc
RM=rm -rf
MKDIR=mkdir -p
CFLAGS=-Wall -Werror -Wextra $(INCLUDES)
DEBUG_FLAGS=-g3

all: $(NAME)
debug: CFLAGS+= $(DEBUG_FLAGS)
debug: all
clean:
	$(RM) $(OBJS_DIR)
fclean: clean
	$(RM) $(NAME)
re: fclean all

$(NAME): $(OBJS)
	$(CC) $(OBJS) -o $@
$(OBJS_DIR)/%.o: $(SRCS_DIR)/%.c | $(OBJS_DIR)
	$(CC) $(CFLAGS) -c $< -o $@
$(OBJS_DIR):
	$(MKDIR) $@

.PHONY: all clean fclean re debug