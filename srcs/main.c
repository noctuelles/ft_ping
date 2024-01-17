/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/16 10:28:19 by plouvel           #+#    #+#             */
/*   Updated: 2024/01/17 17:57:24 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>

#include "ft_args_parser.h"
#include "ft_args_parser_fn.h"
#include "ft_ping.h"
#include "icmp.h"

static t_args_parser_option_entry g_parser_entries[] = {
    {.short_key                     = "c",
     .long_key                      = "count",
     .argument                      = true,
     .long_key_argument_description = "NUMBER",
     .parse_fn                      = parse_count,
     .description                   = "stop after sending NUMBER packets"},

    {.short_key                     = "l",
     .long_key                      = "preload",
     .argument                      = true,
     .long_key_argument_description = "NUMBER",
     .parse_fn                      = parse_preload,
     .description = "send NUMBER packets as fast as possible before falling into normal mode of behavior"},

    {.short_key                     = "?",
     .long_key                      = "help",
     .argument                      = false,
     .long_key_argument_description = NULL,
     .parse_fn                      = parse_help,
     .description                   = "display help list"},

    {.short_key                     = NULL,
     .long_key                      = "ttl",
     .argument                      = true,
     .long_key_argument_description = "N",
     .parse_fn                      = NULL,
     .description                   = "specify N as time-to-live"},

    {.short_key                     = "n",
     .long_key                      = "numeric",
     .argument                      = false,
     .long_key_argument_description = NULL,
     .parse_fn                      = NULL,
     .description                   = "do not resolve host addresses"},

    {.short_key                     = "w",
     .long_key                      = "timeout",
     .argument                      = true,
     .long_key_argument_description = "N",
     .parse_fn                      = NULL,
     .description                   = "stop after N seconds"},

    {.short_key                     = "s",
     .long_key                      = "size",
     .argument                      = true,
     .long_key_argument_description = "NUMBER",
     .parse_fn                      = parse_packet_size,
     .description                   = "send NUMBER data octets"}};

static t_args_parser_config g_args_parser_config = {
    .parser_entries     = g_parser_entries,
    .nbr_parser_entries = sizeof(g_parser_entries) / sizeof(t_args_parser_option_entry),
    .parse_argument_fn  = parse_arguments,
};

int
main(int argc, char **argv) {
    t_ft_ping ft_ping = {
        .options_value.packet_data_size = 56,
    };

    g_args_parser_config.input = &ft_ping;
    g_args_parser_config.argc  = argc;
    g_args_parser_config.argv  = argv;

    if (ft_args_parser(&g_args_parser_config) == -1) {
        return (1);
    }

    return (0);
}
