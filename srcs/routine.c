/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/20 23:20:19 by plouvel           #+#    #+#             */
/*   Updated: 2024/02/05 11:38:20 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "ft_ping.h"
#include "icmp/echo.h"
#include "icmp/utils.h"
#include "libft.h"
#include "output.h"
#include "utils/wrapper.h"

static int
recv_packet(t_ft_ping *ft_ping, uint8_t *buffer, size_t buffer_size, t_packet_info *pi) {
    int result = 0;

    result = icmp_packet_decode(buffer, buffer_size, pi);
    if (result == -1) {
        fprintf(stderr, "packet too short (%lu bytes) from %s\n", buffer_size, ft_ping->sockaddr.sender_presentation);
        return (-1);
    }
    if (result == 1) {
        fprintf(stderr, "checksum mismatch from %s\n", ft_ping->sockaddr.sender_presentation);
    }
    if (pi->icmp->icmp_type == ICMP_ECHOREPLY) {
        if (ntohs(pi->icmp->icmp_hun.ih_idseq.icd_id) != ft_ping->icmp.seq.id) {
            return (-1);
        }
        if (CHK_SEQ_NBR(ft_ping, ntohs(pi->icmp->icmp_hun.ih_idseq.icd_seq))) {
            pi->duplicate = true;
        } else {
            SET_SEQ_NBR(ft_ping, ntohs(pi->icmp->icmp_hun.ih_idseq.icd_seq));
            pi->duplicate = false;
        }
        return (0);
    }
    return (-1);
}

bool
can_send_packet(const t_ft_ping *ft_ping) {
    if (HAS_OPT(ft_ping, OPT_COUNT) && ft_ping->stat.packet_sent >= ft_ping->options_value.count) {
        return (false);
    }

    return (true);
}

static bool
should_continue(t_ft_ping *ft_ping, int *flags) {
    if (g_ping_state != RUNNING) {
        return (false);
    }
    if (HAS_OPT(ft_ping, OPT_COUNT)) {
        if (ft_ping->stat.packet_sent >= ft_ping->options_value.count) {
            struct timeval tv = {0};

            *flags = 0; /* In case we're preloading packets */
            setsockopt_w(ft_ping->sock_fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)); /* Disable*/

            if (ft_ping->stat.packet_received >= ft_ping->options_value.count) {
                return (false);
            } else {
                if (HAS_OPT(ft_ping, OPT_LINGER)) {
                    struct itimerval it = {0};

                    it.it_value.tv_sec = ft_ping->options_value.linger;

                    setitimer(ITIMER_REAL, &it, NULL);
                }
            }
        }

        if (ft_ping->stat.packet_sent >= ft_ping->options_value.count &&
            ft_ping->stat.packet_received >= ft_ping->options_value.count) {
            return (false);
        }
    }
    return (true);
}

/**
 * @brief After the preload phase, we use a receive timeout to allow sending packets at a regular interval. This is the
 * normal behavior of the ping program. In flood mode, we use a 10ms timeout to allow sending packets as fast as
 * possible.
 *
 * @param ft_ping
 * @param flags pointer to the flags of the recvmsg call.
 * @return int -1 on error, 0 on success.
 */
static int
set_normal_behavior(t_ft_ping *ft_ping, int *flags) {
    struct timeval tv = {0};

    if (HAS_OPT(ft_ping, OPT_FLOOD)) {
        tv.tv_usec = 10000;
    } else {
        tv.tv_sec = ft_ping->options_value.interval_between_packets;
    }
    if (setsockopt_w(ft_ping->sock_fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) == -1) {
        return (-1);
    }
    *flags = 0;

    return (0);
}

/**
 * @brief The main ping routine.
 *
 * @param ft_ping
 * @return int
 */
static int
ft_ping_main_loop(t_ft_ping *ft_ping) {
    struct msghdr msg                       = {0};
    struct iovec  iovec[1]                  = {0};
    uint8_t       recv_buffer[IP_MAXPACKET] = {0};
    ssize_t       recv_buffer_size          = 0;
    t_packet_info pi                        = {0};
    int           flags                     = MSG_DONTWAIT;

    iovec[0].iov_base = recv_buffer;
    iovec[0].iov_len  = sizeof(recv_buffer);
    msg.msg_iov       = iovec;
    msg.msg_iovlen    = sizeof(iovec) / sizeof(struct iovec);
    msg.msg_name      = &ft_ping->sockaddr.sender;
    msg.msg_namelen   = ft_ping->sockaddr.len;

    while (should_continue(ft_ping, &flags) == true) {
        if ((recv_buffer_size = recvmsg_w(ft_ping->sock_fd, &msg, flags)) == -1) {
            return (-1);
        } else if (recv_buffer_size == RECVMSGW_WOULD_BLOCK) {
            if (flags == MSG_DONTWAIT && --ft_ping->options_value.preload_nbr_packets == 0) {
                if (set_normal_behavior(ft_ping, &flags) == -1) {
                    return (-1);
                }
            }

            if (sendto_w(ft_ping->sock_fd, ft_ping->icmp.packet, ft_ping->icmp.packet_size, 0, &ft_ping->sockaddr.host,
                         ft_ping->sockaddr.len) == -1) {
                return (-1);
            } else {
                ft_ping->stat.packet_sent++;
            }
        } else if (recv_buffer_size > 0) {
            if (recv_packet(ft_ping, recv_buffer, recv_buffer_size, &pi) == 0) {
                ft_ping_on_recv_update_stat(&ft_ping->stat, &pi);

                print_echo(ft_ping, &pi);
            }
        }
    }
    return (0);
}

int
ft_ping_routine(t_ft_ping *ft_ping) {
    struct itimerval it = {0};

    print_introduction(ft_ping);
    if (HAS_OPT(ft_ping, OPT_TIMEOUT)) {
        it.it_value.tv_sec = ft_ping->options_value.timeout;

        setitimer(ITIMER_REAL, &it, NULL);
    }
    if (ft_ping_main_loop(ft_ping) == -1) {
        return (-1);
    }
    print_outroduction(ft_ping);

    return (0);
}