/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   linger.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/28 10:25:34 by plouvel           #+#    #+#             */
/*   Updated: 2024/02/11 20:24:36 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_args_parser.h"
#include "ft_ping.h"
#include "libft.h"

int
parse_linger(char *argument, t_args_parser_state *parser_state, void *input) {
    t_ft_ping *ft_ping = (t_ft_ping *)input;

    if (!ft_strmatch_int(argument)) {
        parser_state->error_message = "argument must be a positive integer";
        return (-1);
    }

    ft_ping->options.linger       = true;
    ft_ping->options_value.linger = (time_t)ft_atoi(argument);

    return (0);
}