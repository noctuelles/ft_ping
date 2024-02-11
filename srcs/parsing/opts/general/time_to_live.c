/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time_to_live.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/28 02:12:46 by plouvel           #+#    #+#             */
/*   Updated: 2024/02/11 20:24:58 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <netinet/ip.h>

#include "ft_args_parser.h"
#include "ft_ping.h"
#include "libft.h"

#define IP_MAXTTL UINT8_MAX

int
parse_time_to_live(char *argument, t_args_parser_state *parser_state, void *input) {
    t_ft_ping *ft_ping = (t_ft_ping *)input;

    if (!ft_strmatch_int(argument)) {
        parser_state->error_message = "argument must be an integer";
        return (-1);
    }
    ft_ping->options.time_to_live              = true;
    ft_ping->options_value.packet_time_to_live = (size_t)ft_atoi(argument);
    if (ft_ping->options_value.packet_time_to_live > IP_MAXTTL) {
        parser_state->error_message = "option value too big";
        return (-1);
    }
    return (0);
}