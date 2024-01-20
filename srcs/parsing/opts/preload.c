/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   preload.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/20 14:45:24 by plouvel           #+#    #+#             */
/*   Updated: 2024/01/20 14:45:52 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_args_parser.h"
#include "ft_ping.h"
#include "libft.h"

int
parse_preload(char *argument, t_args_parser_state *parser_state, void *input) {
    t_ft_ping *ft_ping = (t_ft_ping *)input;

    if (!ft_strmatch_int(argument)) {
        parser_state->error_message = "argument must be an integer";
        return (-1);
    }

    TOGGLE_OPTION(ft_ping->option_flags, FT_PING_OPTION_PRELOAD);
    ft_ping->options_value.preload_nbr_packets = ft_atoi(argument);

    return (0);
}