/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ping.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/30 09:19:44 by plouvel           #+#    #+#             */
/*   Updated: 2024/02/02 04:47:00 by plouvel          ###   ########.fr       */
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
    return (0);
}

void
ft_ping_clean(t_ft_ping *ft_ping) {
    if (ft_ping->sock_fd != -1) {
        (void)close(ft_ping->sock_fd); /* Not checking return of close because we're using raw socket */
    }
    if (ft_ping->icmp.packet != NULL) {
        free(ft_ping->icmp.packet);
    }
}

void
ft_ping_on_recv_update_stat(t_ft_ping_stat *ft_ping_stat, const t_packet_info *pi) {
    if (pi->duplicate) {
        ft_ping_stat->packet_duplicate++;
    } else {
        ft_ping_stat->packet_received++;
    }

    if (pi->icmp_payload_len - ICMP_MINLEN >= (uint16_t)sizeof(struct timeval)) {
        ft_ping_stat->last_packet_rtt = compute_round_trip_time(pi->icmp);

        if (ft_ping_stat->last_packet_rtt < ft_ping_stat->min_packet_rtt) {
            ft_ping_stat->min_packet_rtt = ft_ping_stat->last_packet_rtt;
        }

        if (ft_ping_stat->last_packet_rtt > ft_ping_stat->max_packet_rtt) {
            ft_ping_stat->max_packet_rtt = ft_ping_stat->last_packet_rtt;
        }

        ft_ping_stat->avg_packet_rtt += ft_ping_stat->last_packet_rtt;
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
    puts(ft_ping->node);
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
