/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   flood.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/28 11:09:53 by plouvel           #+#    #+#             */
/*   Updated: 2024/01/28 11:10:02 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_args_parser.h"
#include "ft_ping.h"
#include "libft.h"

int
parse_flood(char *argument, t_args_parser_state *parser_state, void *input) {
    t_ft_ping *ft_ping = (t_ft_ping *)input;
    (void)argument;
    (void)parser_state;

    TOGGLE_OPT(ft_ping, OPT_FLOOD);

    return (0);
}