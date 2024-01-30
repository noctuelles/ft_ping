/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ping.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/30 09:19:44 by plouvel           #+#    #+#             */
/*   Updated: 2024/01/30 10:03:16 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

#include <arpa/inet.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>

#include "utils/wrapper.h"

static int
ft_ping_init_setsockopt(const t_ft_ping *ft_ping) {
    int ttl        = 0;
    int tos        = 0;
    int dont_route = 0;

    if (HAS_OPT(ft_ping, OPT_TIME_TO_LIVE)) {
        ttl = (int)ft_ping->options_value.packet_time_to_live;
        if (setsocketopt_w(ft_ping->sock_fd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) == -1) {
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
    return (0);
}

static void
ft_ping_init_set_timer_value(t_ft_ping *ft_ping) {
    if (HAS_OPT(ft_ping, OPT_FLOOD)) {
        ft_ping->timer.value.it_value.tv_nsec    = FLOOD_BASE_INTERVAL;
        ft_ping->timer.value.it_interval.tv_nsec = ft_ping->timer.value.it_value.tv_nsec;
    } else {
        ft_ping->timer.value.it_value.tv_sec    = ft_ping->options_value.interval_between_packets;
        ft_ping->timer.value.it_interval.tv_sec = ft_ping->timer.value.it_value.tv_sec;
    }
}

void
ft_ping_clean(t_ft_ping *ft_ping) {
    if (ft_ping->timer.id != -1) {
        (void)timer_delete(ft_ping->timer.id);
    }
    if (ft_ping->sock_fd != -1) {
        (void)close(ft_ping->sock_fd); /* Not checking return of close because we're using raw socket */
    }
}

int
ft_ping_init(t_ft_ping *ft_ping) {
    struct sigevent sigev = {0};
    int             ret   = -1;

    sigev.sigev_notify = SIGEV_SIGNAL;
    sigev.sigev_signo  = SIGALRM;

    ft_ping->sock_fd           = -1;
    ft_ping->timer.id          = -1;
    ft_ping->sockaddr.sock_len = sizeof(struct sockaddr_in);
    ft_ping->icmp.seq.id       = (uint16_t)(getpid() & 0xFFFF);

    if (timer_create_w(CLOCK_MONOTONIC, &sigev, &ft_ping->timer.id) == -1) {
        goto clean;
    }
    ft_ping_init_set_timer_value(ft_ping);
    if ((ft_ping->icmp.packet = malloc_w(ICMP_ECHO_PACKET_SIZE(ft_ping->options_value.packet_data_size))) == NULL) {
        goto clean;
    }
    if ((ft_ping->sock_fd = get_socket_from_node(ft_ping->node, &ft_ping->sockaddr.from)) == -1) {
        goto clean;
    }
    if (ft_ping_init_setsockopt(ft_ping) == -1) {
        goto clean;
    }
    (void)inet_ntop(AF_INET, &((struct sockaddr_in *)&ft_ping->sockaddr.from)->sin_addr,
                    ft_ping->sockaddr.from_presentation, INET_ADDRSTRLEN);
    ret = 0;
clean:
    ft_ping_clean(ft_ping);
    return (ret);
}