/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   size.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/20 14:47:54 by plouvel           #+#    #+#             */
/*   Updated: 2024/01/30 04:19:14 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <netinet/ip.h>
#include <netinet/ip_icmp.h>

#include "ft_args_parser.h"
#include "ft_ping.h"
#include "libft.h"

#define IP_HMINLEN 20

int
parse_size(char *argument, t_args_parser_state *parser_state, void *input) {
    t_ft_ping *ft_ping = (t_ft_ping *)input;

    if (!ft_strmatch_int(argument)) {
        parser_state->error_message = "argument must be an integer";
        return (-1);
    }

    ft_ping->options_value.packet_data_size = (size_t)ft_atoi(argument);
    if (ft_ping->options_value.packet_data_size > IP_MAXPACKET - IP_HMINLEN - ICMP_MINLEN) {
        parser_state->error_message = "option value too big";
        return (-1);
    }

    TOGGLE_OPT(ft_ping, OPT_SIZE);

    return (0);
}