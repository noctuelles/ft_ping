SRCS_DIR=srcs
LIBFT_DIR=libft
OBJS_DIR=objs
INCLUDES_DIR=includes $(LIBFT_DIR)/includes

SRCS=\
		parsing/opts/general/count.c \
		parsing/opts/general/debug.c \
		parsing/opts/general/ignore_routing.c \
		parsing/opts/general/interval.c \
		parsing/opts/general/linger.c \
		parsing/opts/general/numeric.c \
		parsing/opts/general/time_to_live.c \
		parsing/opts/general/timeout.c \
		parsing/opts/general/type_of_service.c \
		parsing/opts/general/verbose.c \
		parsing/opts/echo/data_pattern.c \
		parsing/opts/echo/flood.c \
		parsing/opts/echo/ip_timestamp.c \
		parsing/opts/echo/preload.c \
		parsing/opts/echo/quiet.c \
		parsing/opts/echo/route.c \
		parsing/opts/echo/size.c \
		parsing/opts/argument.c \
		parsing/opts/help.c \
		utils/wrapper.c \
		utils/time.c \
		icmp/echo.c \
		icmp/utils.c \
		ft_ping.c\
		main.c  \
		routine.c \
		output.c \
		translation.c

LIBFT=$(LIBFT_DIR)/libft.a

OBJS=$(addprefix $(OBJS_DIR)/, $(SRCS:.c=.o))
HEADS=$(addprefix -I, $(INCLUDES_DIR))

NAME=ft_ping

CC=gcc
RM=rm -rf
MKDIR=mkdir -p

CFLAGS=-Wall -Werror -Wextra -Wpedantic

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