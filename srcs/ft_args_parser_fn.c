/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_args_parser_fn.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/14 20:46:17 by plouvel           #+#    #+#             */
/*   Updated: 2024/01/17 17:28:48 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_args_parser.h"
#include "ft_ping.h"
#include "libft.h"

static int
match_integer(const char *str) {
    size_t i = 0;

    while (str[i] != '\0') {
        if (!ft_isdigit(str[i])) {
            return (0);
        }
        i++;
    }
    return (1);
}

int
parse_count(const char *argument, t_args_parser_state *parser_state, void *input) {
    t_ft_ping *ft_ping = (t_ft_ping *)input;

    if (!match_integer(argument)) {
        parser_state->error_message = "argument must be an integer";
        return (-1);
    }

    TOGGLE_OPTION(ft_ping->option_flags, FT_PING_OPTION_COUNT);
    ft_ping->options_value.stop_until_nbr_packets = ft_atoi(argument);

    return (0);
}

int
parse_preload(const char *argument, t_args_parser_state *parser_state, void *input) {
    t_ft_ping *ft_ping = (t_ft_ping *)input;

    if (!match_integer(argument)) {
        parser_state->error_message = "argument must be an integer";
        return (-1);
    }

    TOGGLE_OPTION(ft_ping->option_flags, FT_PING_OPTION_PRELOAD);
    ft_ping->options_value.preload_nbr_packets = ft_atoi(argument);

    return (0);
}

int
parse_help(const char *argument, t_args_parser_state *parser_state, void *input) {
    t_ft_ping *ft_ping = (t_ft_ping *)input;

    (void)argument;
    (void)parser_state;

    TOGGLE_OPTION(ft_ping->option_flags, FT_PING_OPTION_HELP);

    return (0);
}

int
parse_packet_size(const char *argument, t_args_parser_state *parser_state, void *input) {
    t_ft_ping *ft_ping = (t_ft_ping *)input;

    if (!match_integer(argument)) {
        parser_state->error_message = "argument must be an integer";
        return (-1);
    }

    ft_ping->options_value.packet_data_size = ft_atoi(argument);

    TOGGLE_OPTION(ft_ping->option_flags, FT_PING_OPTION_PACKET_DATA_SIZE);

    return (0);
}

int
parse_arguments(const char *argument, t_args_parser_state *parser_state, void *input) {
    t_ft_ping *ft_ping = (t_ft_ping *)input;
    (void)parser_state;

    if (ft_ping->target == NULL) {
        ft_ping->target = argument;
    }

    return (0);
}