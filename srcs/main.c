/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/16 10:28:19 by plouvel           #+#    #+#             */
/*   Updated: 2024/02/08 05:22:30 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <arpa/inet.h>
#include <errno.h>
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "ft_args_parser.h"
#include "ft_ping.h"
#include "icmp/echo.h"
#include "parsing/opts/parser_fn.h"
#include "routine.h"
#include "utils/time.h"
#include "utils/wrapper.h"

static t_args_parser_option_entry g_parser_entries[MAX_PARSER_ENTRIES] = {
    /* Option valid for all request types */
    {.short_key                     = "c",
     .long_key                      = "count",
     .argument                      = true,
     .long_key_argument_description = "NUMBER",
     .parse_fn                      = parse_count,
     .description                   = "stop after sending NUMBER packets"},

    {.short_key                     = "d",
     .long_key                      = "debug",
     .argument                      = false,
     .long_key_argument_description = NULL,
     .parse_fn                      = parse_debug,
     .description                   = "set the SO_DEBUG option"},

    {.short_key                     = "i",
     .long_key                      = "interval",
     .argument                      = true,
     .long_key_argument_description = "NUMBER",
     .parse_fn                      = parse_interval,
     .description                   = "wait NUMBER seconds between sending each packet"},

    {.short_key                     = "n",
     .long_key                      = "numeric",
     .argument                      = false,
     .long_key_argument_description = NULL,
     .parse_fn                      = parse_numeric,
     .description                   = "do not resolve host addresses"},

    {.short_key                     = "r",
     .long_key                      = "ignore-routing",
     .argument                      = false,
     .long_key_argument_description = NULL,
     .parse_fn                      = parse_ignore_routing,
     .description                   = "send directly to a host on an attached network"},

    {.short_key                     = NULL,
     .long_key                      = "ttl",
     .argument                      = true,
     .long_key_argument_description = "N",
     .parse_fn                      = parse_time_to_live,
     .description                   = "specify N as time-to-live"},

    {.short_key                     = "T",
     .long_key                      = "tos",
     .argument                      = true,
     .long_key_argument_description = "NUM",
     .parse_fn                      = parse_type_of_service,
     .description                   = "set type of service (TOS) to NUM"},

    {.short_key                     = "v",
     .long_key                      = "verbose",
     .argument                      = false,
     .long_key_argument_description = NULL,
     .parse_fn                      = parse_verbose,
     .description                   = "verbose output"},

    {.short_key                     = "w",
     .long_key                      = "timeout",
     .argument                      = true,
     .long_key_argument_description = "N",
     .parse_fn                      = parse_timeout,
     .description                   = "stop after N seconds"},

    {.short_key                     = "W",
     .long_key                      = "linger",
     .argument                      = true,
     .long_key_argument_description = "N",
     .parse_fn                      = parse_linger,
     .description                   = "number of seconds to wait for response"},

    /* END - Option valid for all request types */

    /* Options valid for ECHO_REQUEST */

    {.short_key                     = "l",
     .long_key                      = "preload",
     .argument                      = true,
     .long_key_argument_description = "NUMBER",
     .parse_fn                      = parse_preload,
     .description = "send NUMBER packets as fast as possible before falling into normal mode of behavior"},

    {.short_key                     = "f",
     .long_key                      = "flood",
     .argument                      = false,
     .long_key_argument_description = NULL,
     .parse_fn                      = parse_flood,
     .description = "send NUMBER packets as fast as possible before falling into normal mode of behavior"},

    {.short_key                     = "?",
     .long_key                      = "help",
     .argument                      = false,
     .long_key_argument_description = NULL,
     .parse_fn                      = parse_help,
     .description                   = "display help list"},

    {.short_key                     = "s",
     .long_key                      = "size",
     .argument                      = true,
     .long_key_argument_description = "NUMBER",
     .parse_fn                      = parse_size,
     .description                   = "send NUMBER data octets"},

    {.short_key                     = "p",
     .long_key                      = "pattern",
     .argument                      = true,
     .long_key_argument_description = "PATTERN",
     .parse_fn                      = parse_pattern,
     .description                   = "fill ICMP packet with given pattern (hex)"},

};

static t_args_parser_config g_args_parser_config = {
    .argc               = 0,
    .argv               = NULL,
    .parser_entries     = g_parser_entries,
    .nbr_parser_entries = sizeof(g_parser_entries) / sizeof(t_args_parser_option_entry),
    .parse_argument_fn  = parse_arguments,
    .input              = NULL,

};

t_ping_state g_ping_state = RUNNING_SEND_PRELOADING;

void
sighandler(int signum) {
    if (signum == SIGALRM) {
        g_ping_state = RUNNING_SEND;
    } else {
        g_ping_state = ENDING;
    }
}

int
main(int argc, char **argv) {
    struct sigaction sigact  = {0};
    t_ft_ping        ft_ping = {0};

    sigact.sa_handler = sighandler;
    (void)sigaction(SIGALRM, &sigact, NULL);
    (void)sigaction(SIGINT, &sigact, NULL);

    ft_ping.options_value.packet_data_size            = DEFAULT_PACKET_DATA_SIZE;
    ft_ping.options_value.ms_interval_between_packets = 1000;
    ft_ping.options_value.preload_nbr_packets         = DEFAULT_PRELOAD_NBR_PACKETS;
    ft_ping.stat.max_packet_rtt                       = -INFINITY;
    ft_ping.stat.min_packet_rtt                       = INFINITY;

    g_args_parser_config.input = &ft_ping;
    g_args_parser_config.argc  = argc;
    g_args_parser_config.argv  = argv;

    if (ft_args_parser(&g_args_parser_config) == -1) {
        return (1);
    }
    if (ft_ping_init(&ft_ping) == -1) {
        return (1);
    }
    if (ft_ping_routine(&ft_ping) == -1) {
        return (1);
    }
    ft_ping_clean(&ft_ping);
    return (0);
}
