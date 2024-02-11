/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ping.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/30 09:19:44 by plouvel           #+#    #+#             */
/*   Updated: 2024/02/11 19:04:02 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

#include <arpa/inet.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "utils/time.h"
#include "utils/wrapper.h"

static int
ft_ping_init_setsockopt(const t_ft_ping *ft_ping) {
    int ttl        = 0;
    int tos        = 0;
    int dont_route = 0;
    int debug      = 0;

    if (HAS_OPT(ft_ping, OPT_TIME_TO_LIVE)) {
        ttl = (int)ft_ping->options_value.packet_time_to_live;
        if (setsockopt_w(ft_ping->sock_fd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) == -1) {
            return (-1);
        }
    }
    if (HAS_OPT(ft_ping, OPT_TYPE_OF_SERVICE)) {
        tos = (int)ft_ping->options_value.packet_type_of_service;
        if (setsockopt_w(ft_ping->sock_fd, IPPROTO_IP, IP_TOS, &tos, sizeof(tos)) == -1) {
            return (-1);
        }
    }
    if (HAS_OPT(ft_ping, OPT_IGNORE_ROUTING)) {
        dont_route = 1;
        if (setsockopt_w(ft_ping->sock_fd, SOL_SOCKET, SO_DONTROUTE, &dont_route, sizeof(dont_route)) == -1) {
            return (-1);
        }
    }
    if (HAS_OPT(ft_ping, OPT_DEBUG)) {
        debug = 1;
        if (setsockopt_w(ft_ping->sock_fd, SOL_SOCKET, SO_DEBUG, &debug, sizeof(debug)) == -1) {
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
    if (ft_ping_init_setsockopt(ft_ping) == -1) {
        goto error;
    }
    (void)inet_ntop(AF_INET, &((struct sockaddr_in *)&ft_ping->sockaddr.host)->sin_addr,
                    ft_ping->sockaddr.host_presentation, INET_ADDRSTRLEN);
    return (0);
error:
    ft_ping_clean(ft_ping);
    return (-1);
}
