/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/16 10:28:19 by plouvel           #+#    #+#             */
/*   Updated: 2024/01/20 14:18:45 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <arpa/inet.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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
     .parse_fn                      = parse_timeout,
     .description                   = "stop after N seconds"},

    {.short_key                     = "s",
     .long_key                      = "size",
     .argument                      = true,
     .long_key_argument_description = "NUMBER",
     .parse_fn                      = parse_packet_size,
     .description                   = "send NUMBER data octets"},

    {.short_key                     = "p",
     .long_key                      = "pattern",
     .argument                      = true,
     .long_key_argument_description = "PATTERN",
     .parse_fn                      = parse_data_pattern,
     .description                   = "fill ICMP packet with given pattern (hex)"}};

static t_args_parser_config g_args_parser_config = {
    .parser_entries     = g_parser_entries,
    .nbr_parser_entries = sizeof(g_parser_entries) / sizeof(t_args_parser_option_entry),
    .parse_argument_fn  = parse_arguments,
};

t_ft_ping g_ft_ping = {0};

int
send_packet(t_socket *socket, t_icmp_packet *packet) {
    if (sendto(socket->fd, packet->payload, packet->payload_size, MSG_NOSIGNAL,
               (struct sockaddr *)socket->addrinfo.ai_addr, socket->addrinfo.ai_addrlen) == -1) {
        fprintf(stderr, "ft_ping: %s\n", strerror(errno));
        return (-1);
    }

    return (0);
}

void
handle_interuption(int sig) {
    (void)sig;

    puts("OK");
    exit(1);
}

int
main(int argc, char **argv) {
    t_ft_ping ft_ping = {0};

    ft_ping.options_value.packet_data_size = DEFAULT_PACKET_DATA_SIZE;

    g_args_parser_config.input = &ft_ping;
    g_args_parser_config.argc  = argc;
    g_args_parser_config.argv  = argv;

    if (ft_args_parser(&g_args_parser_config) == -1) {
        return (1);
    }

    if (get_socket_from_node(ft_ping.target, &ft_ping.socket) == -1) {
        return (1);
    }

    t_icmp_packet packet = {0};

    prepare_icmp_packet(&packet, &ft_ping.options_value.data_pattern, ft_ping.options_value.packet_data_size);

    send_packet(&ft_ping.socket, &packet);

    return (0);
}
