/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   data_pattern.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/20 14:40:48 by plouvel           #+#    #+#             */
/*   Updated: 2024/01/20 14:54:14 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "ft_args_parser.h"
#include "ft_ping.h"
#include "libft.h"

static void
parse_pattern(char *argument, uint8_t *data_pattern) {
    char   hex[] = "0123456789abcdef";
    char   tmp   = 0;
    size_t i     = 0;
    size_t j     = 0;
    size_t k     = 0;

    while (argument[i] != '\0') {
        j = i;
        while (argument[j] != '\0' && j < i + 2) {
            j++;
        }
        tmp               = argument[j];
        argument[j]       = '\0';
        data_pattern[k++] = ft_atoi_base(&argument[i], hex);
        argument[j]       = tmp;
        i                 = j;
    }
}

int
parse_data_pattern(char *argument, t_args_parser_state *parser_state, void *input) {
    t_ft_ping     *ft_ping = (t_ft_ping *)input;
    t_data_pattern data_pattern;
    size_t         argument_size;

    if (!ft_strmatch_hex(argument)) {
        parser_state->error_message = "argument must be an hexadecimal string";
        return (-1);
    }

    argument_size = ft_strlen(argument);

    if (argument_size > 16) {
        argument_size = 16;
        argument[16]  = '\0';
    }

    data_pattern.pattern_size = argument_size / 2 + argument_size % 2;
    if ((data_pattern.pattern = malloc(data_pattern.pattern_size)) == NULL) {
        parser_state->error_message = strerror(errno);
        return (-1);
    }

    parse_pattern(argument, data_pattern.pattern);

    ft_ping->options_value.data_pattern = data_pattern;

    return (0);
}