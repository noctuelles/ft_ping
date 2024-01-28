/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ignore_routing.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/28 10:35:32 by plouvel           #+#    #+#             */
/*   Updated: 2024/01/28 10:35:43 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_args_parser.h"
#include "ft_ping.h"
#include "libft.h"

int
parse_ignore_routing(char *argument, t_args_parser_state *parser_state, void *input) {
    t_ft_ping *ft_ping = (t_ft_ping *)input;
    (void)argument;
    (void)parser_state;

    TOGGLE_OPT(ft_ping, OPT_IGNORE_ROUTING);

    return (0);
}