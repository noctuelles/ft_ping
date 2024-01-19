SRCS_DIR=srcs
LIBFT_DIR=libft
OBJS_DIR=objs
INCLUDES_DIR=includes $(LIBFT_DIR)/includes

SRCS=	main.c  \
		ft_args_parser_fn.c \
		icmp.c \
		translation.c

LIBFT=$(LIBFT_DIR)/libft.a

OBJS=$(addprefix $(OBJS_DIR)/, $(SRCS:.c=.o))
HEADS=$(addprefix -I, $(INCLUDES_DIR))

NAME=ft_ping

CC=gcc
RM=rm -rf
MKDIR=mkdir -p

CFLAGS=-Wall -Werror -Wextra

DEBUG_FLAGS=-g3

all: $(NAME)
clean:
	$(RM) $(OBJS_DIR)
fclean: clean
	$(RM) $(NAME)
re: fclean all

$(NAME): $(LIBFT) $(OBJS)
	$(CC) $(OBJS) $(LIBFT) $(DEBUG_FLAGS) -o $@
$(OBJS_DIR)/%.o: $(SRCS_DIR)/%.c | $(OBJS_DIR)
	$(CC) $(CFLAGS) $(HEADS) $(DEBUG_FLAGS) -c $< -o $@
$(OBJS_DIR):
	$(MKDIR) $@
$(LIBFT):
	$(MAKE) -C $(LIBFT_DIR) all

.PHONY: all clean fclean re