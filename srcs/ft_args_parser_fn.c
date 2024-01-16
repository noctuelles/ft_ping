/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_args_parser_fn.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/14 20:46:17 by plouvel           #+#    #+#             */
/*   Updated: 2024/01/16 10:48:27 by plouvel          ###   ########.fr       */
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

    ft_ping->option_flags |= FT_PING_OPTION_COUNT;
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

    ft_ping->option_flags |= FT_PING_OPTION_PRELOAD;
    ft_ping->options_value.preload_nbr_packets = ft_atoi(argument);

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