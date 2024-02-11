/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   data_pattern.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/20 14:40:48 by plouvel           #+#    #+#             */
/*   Updated: 2024/02/11 20:18:30 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "ft_args_parser.h"
#include "ft_ping.h"
#include "libft.h"

#define HEX_STR "0123456789abcdef"
#define HEX_STR_PAIR_SIZE 2

/**
 * @brief parse the hexadecimal string and store it in pattern.
 * @note the hexadecimal string is parsed by pair of two characters, each pair is converted to a byte.
 *
 * @param argument the current argument
 * @param pattern pointer to data_pattern array
 */
static void
fill_pattern(char *argument, uint8_t *pattern) {
    char   hex[] = HEX_STR;
    char   tmp   = 0;
    size_t i = 0, j = 0, k = 0;

    while (argument[i] != '\0') {
        while (argument[j] != '\0' && j < i + HEX_STR_PAIR_SIZE) {
            j++;
        }
        tmp          = argument[j];
        argument[j]  = '\0';
        pattern[k++] = ft_atoi_base(&argument[i], hex);
        argument[j]  = tmp;
        i            = j;
    }
}

static void
lower_str(unsigned int i, char *str) {
    (void)i;
    *str = ft_tolower(*str);
}

/**
 * @note A data pattern is a maximum 16 bytes pattern that will fill the data section of the ICMP packet.
 */
int
parse_pattern(char *argument, t_args_parser_state *parser_state, void *input) {
    t_ft_ping     *ft_ping = (t_ft_ping *)input;
    t_data_pattern data_pattern;
    size_t         argument_size;

    if (!ft_strmatch_hex(argument)) {
        parser_state->error_message = "argument must be an hexadecimal string";
        return (-1);
    }

    ft_striteri(argument, lower_str);

    argument_size = ft_strlen(argument);
    if (argument_size > PATTERN_MAX_SIZE) {
        argument_size              = PATTERN_MAX_SIZE;
        argument[PATTERN_MAX_SIZE] = '\0';
    }
    data_pattern.pattern_size = argument_size / HEX_STR_PAIR_SIZE + argument_size % HEX_STR_PAIR_SIZE;

    fill_pattern(argument, data_pattern.pattern);

    ft_ping->options.pattern                   = true;
    ft_ping->options_value.packet_data_pattern = data_pattern;

    return (0);
}