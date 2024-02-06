/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   interval.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/20 23:13:14 by plouvel           #+#    #+#             */
/*   Updated: 2024/02/06 05:18:01 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <errno.h>
#include <stdlib.h>

#include "ft_args_parser.h"
#include "ft_ping.h"
#include "libft.h"
#include "utils/time.h"

int
parse_interval(char *argument, t_args_parser_state *parser_state, void *input) {
    t_ft_ping *ft_ping = (t_ft_ping *)input;
    double     timeout = 0;
    char      *endptr  = NULL;

    errno   = 0;
    timeout = strtod(argument, &endptr);
    if (errno != 0 || *endptr != '\0') {
        parser_state->error_message = "invalid value";
        return (-1);
    }
    if (timeout < 0.2) {
        parser_state->error_message = "option value too small";
        return (-1);
    }
    TOGGLE_OPT(ft_ping, OPT_INTERVAL);
    ft_ping->options_value.interval_between_packets = get_timeval_from_secs(timeout);

    return (0);
}