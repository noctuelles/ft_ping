/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   watchdog.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/08 13:24:49 by plouvel           #+#    #+#             */
/*   Updated: 2024/02/11 20:34:52 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <time.h>

#include "ft_ping.h"
#include "icmp/echo.h"
#include "icmp/utils.h"
#include "utils/wrapper.h"

static int
setsock_recvtimeout(int sockfd, time_t timeout_in_secs) {
    struct timeval tv = {0};

    tv.tv_sec = timeout_in_secs;
    if (setsockopt_w(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) == -1) {
        return (-1);
    }

    return (0);
}

static bool
should_be_finished(const struct timespec *start_time, time_t timeout_in_secs) {
    struct timespec now                  = {0};
    uint64_t        amount_of_time_in_ms = 0;

    (void)clock_gettime(CLOCK_MONOTONIC, &now);
    amount_of_time_in_ms = (now.tv_sec - start_time->tv_sec) * 1000 + (now.tv_nsec - start_time->tv_nsec) / 1e6;

    return (amount_of_time_in_ms >= (uint64_t)timeout_in_secs * 1000);
}

/**
 * @brief
 * @note This function is used to determine if the program should continue to run or not, and manage the program
 * behavior regarding in which state we're in.
 */
int
watchdog(t_ft_ping *ft_ping) {
    if (g_ping_state == FINISHING) {
        return (0);
    }
    if (ft_ping->options.timeout) {
        if (should_be_finished(&ft_ping->start_time, ft_ping->options_value.timeout)) {
            return (0);
        }
    }
    if (ft_ping->options.count) {
        if (ft_ping->stat.packet_sent >= ft_ping->options_value.count) {
            if (g_ping_state >= RUNNING_SEND) {
                g_ping_state = RUNNING_RECV;
            }
            if (ft_ping->options.linger) {
                if (setsock_recvtimeout(ft_ping->sock_fd, ft_ping->options_value.linger) == -1) {
                    return (-1);
                }
            }
            if (ft_ping->stat.packet_received >= ft_ping->options_value.count) {
                return (0);
            }
        }
    }
    if (g_ping_state >= RUNNING_SEND) {
        fill_icmp_echo_packet_header((struct icmphdr *)ft_ping->icmp.packet, ft_ping->icmp.seq.nbr,
                                     ft_ping->icmp.seq.id);
        fill_icmp_echo_packet_data(ft_ping->icmp.packet->icmp_dun.id_data, &ft_ping->options_value.packet_data_pattern,
                                   ft_ping->options_value.packet_data_size);
        ft_ping->icmp.packet->icmp_cksum = icmp_checksum(ft_ping->icmp.packet, ft_ping->icmp.packet_size);

        if (sendto_w(ft_ping->sock_fd, ft_ping->icmp.packet, ft_ping->icmp.packet_size, 0, &ft_ping->sockaddr.host,
                     ft_ping->sockaddr.len) == -1) {
            return (-1);
        } else {
            ft_ping->stat.packet_sent++;
            ft_ping->icmp.seq.nbr++;
        }

        if (g_ping_state == RUNNING_SEND_PRELOADING) {
            ft_ping->options_value.preload_nbr_packets--;
            if (ft_ping->options_value.preload_nbr_packets == 0) {
                g_ping_state = RUNNING_SEND;
            } else {
                return (1);
            }
        }
        if (g_ping_state == RUNNING_SEND) {
            (void)timer_settime(ft_ping->timer_id, 0, &ft_ping->timer_spec, NULL);
        }
        g_ping_state = RUNNING_RECV;
    }

    return (true);
}