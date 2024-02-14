/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ping.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/30 09:19:44 by plouvel           #+#    #+#             */
/*   Updated: 2024/02/14 23:17:20 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "ft_args_parser.h"
#include "utils/wrapper.h"

void print_intro(const t_ft_ping *ft_ping);
void print_outro(const t_ft_ping *ft_ping);
static int
ft_ping_init_setsockopt(int sock_fd, const t_ft_ping_options *ft_ping_options,
                        const t_ft_ping_options_value *ft_ping_options_value) {
    int ttl        = 0;
    int tos        = 0;
    int dont_route = 0;
    int debug      = 0;

    if (ft_ping_options->time_to_live) {
        ttl = (int)ft_ping_options_value->packet_time_to_live;
        if (setsockopt_w(sock_fd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) == -1) {
            return (-1);
        }
    }
    if (ft_ping_options->type_of_service) {
        tos = (int)ft_ping_options_value->packet_type_of_service;
        if (setsockopt_w(sock_fd, IPPROTO_IP, IP_TOS, &tos, sizeof(tos)) == -1) {
            return (-1);
        }
    }
    if (ft_ping_options->ignore_routing) {
        dont_route = 1;
        if (setsockopt_w(sock_fd, SOL_SOCKET, SO_DONTROUTE, &dont_route, sizeof(dont_route)) == -1) {
            return (-1);
        }
    }
    if (ft_ping_options->debug) {
        debug = 1;
        if (setsockopt_w(sock_fd, SOL_SOCKET, SO_DEBUG, &debug, sizeof(debug)) == -1) {
            return (-1);
        }
    }
    return (0);
}

static int
ft_ping_init_timer(t_ft_ping *ft_ping) {
    struct sigevent sev = {0};

    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo  = SIGALRM;

    if (timer_create_w(CLOCK_MONOTONIC, &sev, &ft_ping->timer_id) == -1) {
        return (-1);
    }

    return (0);
}

void
ft_ping_clean(t_ft_ping *ft_ping) {
    if (ft_ping->icmp.packet != NULL) {
        free(ft_ping->icmp.packet);
    }
    (void)timer_delete(ft_ping->timer_id);
    if (ft_ping->sock_fd != -1) {
        (void)close(ft_ping->sock_fd); /* Not checking return of close because we're using raw socket */
    }
}

int
ft_ping_init(t_ft_ping *ft_ping) {
    ft_ping->sock_fd          = -1;
    ft_ping->sockaddr.len     = sizeof(struct sockaddr_in);
    ft_ping->icmp.seq.id      = (uint16_t)(getpid() & 0xFFFF);
    ft_ping->icmp.packet_size = ICMP_MINLEN + ft_ping->options_value.packet_data_size;

    if ((ft_ping->icmp.packet = malloc_w(ft_ping->icmp.packet_size)) == NULL) {
        goto error;
    }
    if (ft_ping_init_timer(ft_ping) == -1) {
        goto error;
    }
    if ((ft_ping->sock_fd = get_socket_from_node(ft_ping->node, &ft_ping->sockaddr.host)) == -1) {
        goto error;
    }
    if (ft_ping_init_setsockopt(ft_ping->sock_fd, &ft_ping->options, &ft_ping->options_value) == -1) {
        goto error;
    }
    (void)inet_ntop(AF_INET, &((struct sockaddr_in *)&ft_ping->sockaddr.host)->sin_addr,
                    ft_ping->sockaddr.host_presentation, INET_ADDRSTRLEN);
    return (0);
error:
    ft_ping_clean(ft_ping);
    return (-1);
}

void
ft_ping_print_intro(const t_ft_ping *ft_ping) {
    (void)printf("PING %s (%s): %lu data bytes", ft_ping->node, ft_ping->sockaddr.host_presentation,
                 ft_ping->options_value.packet_data_size);
    if (ft_ping->options.verbose) {
        (void)printf(", id %#04x = %u", ft_ping->icmp.seq.id, ft_ping->icmp.seq.id);
    }
    (void)printf("\n");
}

void
ft_ping_print_outro(const t_ft_ping *ft_ping) {
    (void)printf("--- %s ping statistics ---\n", ft_ping->node);

    (void)printf("%lu packets transmitted, %lu received", ft_ping->stat.packet_sent, ft_ping->stat.packet_received);
    if (ft_ping->stat.packet_duplicate != 0) {
        (void)printf(", +%lu duplicates", ft_ping->stat.packet_duplicate);
    }
    (void)printf(", %lu%% packet loss",
                 (ft_ping->stat.packet_sent - ft_ping->stat.packet_received) * 100 / ft_ping->stat.packet_sent);
    (void)printf("\n");

    if (ft_ping->stat.last_packet_rtt != 0.0) {
        (void)printf("round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\n", ft_ping->stat.min_packet_rtt,
                     ft_ping->stat.avg_packet_rtt / (double)ft_ping->stat.packet_received, ft_ping->stat.max_packet_rtt,
                     0.0f);
    }
}