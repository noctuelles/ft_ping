/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   type_of_service.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/28 10:36:35 by plouvel           #+#    #+#             */
/*   Updated: 2024/01/28 10:41:11 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_args_parser.h"
#include "ft_ping.h"
#include "libft.h"

#define TOSMAX UINT8_MAX

int
parse_type_of_service(char *argument, t_args_parser_state *parser_state, void *input) {
    t_ft_ping *ft_ping = (t_ft_ping *)input;

    if (!ft_strmatch_int(argument)) {
        parser_state->error_message = "argument must be a positive integer";
        return (-1);
    }
    TOGGLE_OPT(ft_ping, OPT_TYPE_OF_SERVICE);
    ft_ping->options_value.packet_type_of_service = (uint64_t)ft_atoi(argument);
    if (ft_ping->options_value.packet_type_of_service > TOSMAX) {
        parser_state->error_message = "option value too big";
        return (-1);
    }
    return (0);
}