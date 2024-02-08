/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/20 23:20:19 by plouvel           #+#    #+#             */
/*   Updated: 2024/02/08 05:34:04 by plouvel          ###   ########.fr       */
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
#include <time.h>
#include <unistd.h>

#include "ft_ping.h"
#include "icmp/echo.h"
#include "icmp/utils.h"
#include "libft.h"
#include "output.h"
#include "utils/time.h"
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

/**
 * @brief
 * @note This function is used to determine if the program should continue to run or not, and manage the program
 * behavior regarding in which state we're in.
 *
 * @param ft_ping
 * @param flags pointer to the flags of the recvmsg call.
 * @return true if the program should end.
 * @return false if the program should still run.
 */
static bool
watchdog(t_ft_ping *ft_ping) {
    if (g_ping_state == ENDING) {
        return (false);
    }

    if (HAS_OPT(ft_ping, OPT_TIMEOUT)) {
        struct timespec now = {0};

        (void)clock_gettime(CLOCK_MONOTONIC, &now);

        time_t amount_of_time_in_ms =
            (now.tv_sec - ft_ping->start_time.tv_sec) * 1000 + (now.tv_nsec - ft_ping->start_time.tv_nsec) / 1e6;

        if (amount_of_time_in_ms >= ft_ping->options_value.timeout * 1000) {
            return (false);
        }
    }

    if (HAS_OPT(ft_ping, OPT_COUNT)) {
        if (ft_ping->stat.packet_sent >= ft_ping->options_value.count) {
            if (g_ping_state == RUNNING_SEND) {
                struct itimerspec its = {0};
                timer_settime(ft_ping->timer_id, 0, &its, NULL);
                g_ping_state = RUNNING_RECV;
            }

            if (HAS_OPT(ft_ping, OPT_LINGER)) {
                struct timespec tv = {0};

                tv.tv_sec = ft_ping->options_value.linger;
                (void)setsockopt_w(ft_ping->sock_fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
            }

            if (ft_ping->stat.packet_received >= ft_ping->options_value.count) {
                return (false);
            }
        }
    }

    if (g_ping_state == RUNNING_SEND || g_ping_state == RUNNING_SEND_PRELOADING) {
        fill_icmp_echo_packet_header((struct icmphdr *)ft_ping->icmp.packet, ft_ping->icmp.seq.nbr,
                                     ft_ping->icmp.seq.id);
        fill_icmp_echo_packet_data(ft_ping->icmp.packet->icmp_dun.id_data, &ft_ping->options_value.packet_data_pattern,
                                   ft_ping->options_value.packet_data_size);
        ft_ping->icmp.packet->icmp_cksum = icmp_checksum(ft_ping->icmp.packet, ft_ping->icmp.packet_size);

        if (sendto_w(ft_ping->sock_fd, ft_ping->icmp.packet, ft_ping->icmp.packet_size, 0, &ft_ping->sockaddr.host,
                     ft_ping->sockaddr.len) == -1) {
            return (false);
        } else {
            ft_ping->stat.packet_sent++;
            ft_ping->icmp.seq.nbr++;
        }

        if (g_ping_state == RUNNING_SEND_PRELOADING) {
            ft_ping->options_value.preload_nbr_packets--;
            if (ft_ping->options_value.preload_nbr_packets == 0) {
                struct itimerspec its = {0};

                its.it_interval.tv_sec  = ft_ping->options_value.ms_interval_between_packets / 1000;
                its.it_interval.tv_nsec = (ft_ping->options_value.ms_interval_between_packets % 1000) * 1e6;
                its.it_value            = its.it_interval;

                timer_settime(ft_ping->timer_id, 0, &its, NULL);
            } else {
                return (true);
            }
        }
        g_ping_state = RUNNING_RECV;
    }

    return (true);
}

/**
 * @brief Main ping routine.
 * @note
 * There's a global variable that is used to track if the ping routine should continue to run or not : we're using
 * signal handler to globally modify the program state.
 * We'are using the recvmsg timeout blocking ability to send packages at a regular interval (SO_RCVTIMEO).
 *
 * The routine is divided into 3 states:
 * - RUNNING_PRELOAD: We send a burst of packets as fast as possible. Note that we use MSG_DONTWAIT during preloading to
 *   avoid blocking at the recvmsg call.
 * - RUNNING : We send packets at a regular interval.
 * - RUNNING_SEND_DISABLE : We stop sending packets, but we still wait for the remaining packets to be received. This
 *   state is usefull when the user specified the --count option.
 *
 * @param ft_ping pointer to the ping structure.
 * @return int -1 on error, 0 on success.
 */
static int
ft_ping_main_loop(t_ft_ping *ft_ping) {
    struct msghdr msg                       = {0};
    struct iovec  iovec[1]                  = {0};
    uint8_t       recv_buffer[IP_MAXPACKET] = {0};
    ssize_t       recv_buffer_size          = 0;
    t_packet_info pi                        = {0};

    iovec[0].iov_base = recv_buffer;
    iovec[0].iov_len  = sizeof(recv_buffer);
    msg.msg_iov       = iovec;
    msg.msg_iovlen    = sizeof(iovec) / sizeof(struct iovec);
    msg.msg_name      = &ft_ping->sockaddr.sender;
    msg.msg_namelen   = ft_ping->sockaddr.len;

    while (watchdog(ft_ping) == true) {
        if (g_ping_state == RUNNING_RECV) {
            if ((recv_buffer_size = recvmsg_w(ft_ping->sock_fd, &msg, 0)) == -1) {
                return (-1);
            } else if (recv_buffer_size == RECVMSGW_WOULD_BLOCK) {
                g_ping_state = ENDING;
            } else if (recv_buffer_size > 0) {
                if (recv_packet(ft_ping, recv_buffer, recv_buffer_size, &pi) == 0) {
                    ft_ping_on_recv_update_stat(&ft_ping->stat, &pi);
                    print_echo(ft_ping, &pi);
                }
            }
        }
    }
    return (0);
}

int
ft_ping_routine(t_ft_ping *ft_ping) {
    print_introduction(ft_ping);
    (void)clock_gettime(CLOCK_MONOTONIC, &ft_ping->start_time);
    if (ft_ping_main_loop(ft_ping) == -1) {
        return (-1);
    }
    print_outroduction(ft_ping);

    return (0);
}