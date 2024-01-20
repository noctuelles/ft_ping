SRCS_DIR=srcs
LIBFT_DIR=libft
OBJS_DIR=objs
INCLUDES_DIR=includes $(LIBFT_DIR)/includes

SRCS=	parsing/opts/count.c \
		parsing/opts/data_pattern.c \
		parsing/opts/help.c \
		parsing/opts/packet_size.c \
		parsing/opts/preload.c \
		parsing/opts/timeout.c \
		parsing/opts/argument.c \
		main.c  \
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
$(OBJS_DIR)/%.o: $(SRCS_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(HEADS) $(DEBUG_FLAGS) -c $< -o $@
$(LIBFT):
	$(MAKE) -C $(LIBFT_DIR) all

.PHONY: all clean fclean re