/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/20 23:20:19 by plouvel           #+#    #+#             */
/*   Updated: 2024/02/11 20:35:36 by plouvel          ###   ########.fr       */
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
#include "icmp/default.h"
#include "icmp/echo.h"
#include "icmp/utils.h"
#include "libft.h"
#include "output.h"
#include "utils/net.h"
#include "utils/time.h"
#include "utils/wrapper.h"
#include "watchdog.h"

static int
recv_incoming_packet(t_ft_ping *ft_ping, uint8_t *buffer, size_t buffer_size, t_incoming_packet_info *pi) {
    t_sockaddr_str sockaddr_str;
    int            ret = 0;

    if (sockaddr2str(&pi->from.sockaddr, pi->from.socklen, &sockaddr_str, true) == SOCKADDR2STR_ERR) {
        return (-1);
    }

    ret = icmp_packet_decode(buffer, buffer_size, pi);
    if (ret == -1) {
        fprintf(stderr, "packet too short (%lu bytes) from %s\n", buffer_size, sockaddr_str.ip_str);
        return (0);
    }
    if (ret == 1) {
        fprintf(stderr, "checksum mismatch from %s\n", sockaddr_str.ip_str);
    }

    switch (pi->icmp->icmp_type) {
        case ICMP_ECHOREPLY: {
            bool duplicate = false;

            if (ntohs(pi->icmp->icmp_hun.ih_idseq.icd_id) != ft_ping->icmp.seq.id) {
                return (-1);
            }

            if (CHK_SEQ_NBR(ft_ping, ntohs(pi->icmp->icmp_hun.ih_idseq.icd_seq))) {
                duplicate = true;
            } else {
                SET_SEQ_NBR(ft_ping, ntohs(pi->icmp->icmp_hun.ih_idseq.icd_seq));
                duplicate = false;
            }

            if (duplicate) {
                ft_ping->stat.packet_duplicate++;
            } else {
                ft_ping->stat.packet_received++;
            }

            if (pi->icmp_payload_len - ICMP_MINLEN >= (uint16_t)sizeof(struct timeval)) {
                ft_ping->stat.last_packet_rtt = compute_round_trip_time(pi->icmp);
                if (ft_ping->stat.last_packet_rtt < ft_ping->stat.min_packet_rtt) {
                    ft_ping->stat.min_packet_rtt = ft_ping->stat.last_packet_rtt;
                }
                if (ft_ping->stat.last_packet_rtt > ft_ping->stat.max_packet_rtt) {
                    ft_ping->stat.max_packet_rtt = ft_ping->stat.last_packet_rtt;
                }
                ft_ping->stat.avg_packet_rtt += ft_ping->stat.last_packet_rtt;
            }

            print_icmp_echo_reply(ft_ping, pi, duplicate);

            break;
        }
        default:
            print_icmp_default(pi, ft_ping->options.verbose, ft_ping->options.numeric);
    }
    return (-1);
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
    struct msghdr          msg                       = {0};
    struct iovec           iovec[1]                  = {0};
    uint8_t                recv_buffer[IP_MAXPACKET] = {0};
    ssize_t                ret                       = 0;
    t_incoming_packet_info pi                        = {0};

    iovec[0].iov_base = recv_buffer;
    iovec[0].iov_len  = sizeof(recv_buffer);
    msg.msg_iov       = iovec;
    msg.msg_iovlen    = sizeof(iovec) / sizeof(struct iovec);
    msg.msg_name      = &pi.from;
    msg.msg_namelen   = sizeof(pi.from);

    while ((ret = watchdog(ft_ping)) == 1) {
        if (g_ping_state == RUNNING_RECV) {
            if ((ret = recvmsg_w(ft_ping->sock_fd, &msg, 0)) == -1) {
                return (-1);
            } else if (ret == RECVMSGW_TIMEOUT) {
                g_ping_state = FINISHING;
            } else if (ret > 0) {
                pi.from.socklen = msg.msg_namelen;
                if (recv_incoming_packet(ft_ping, recv_buffer, ret, &pi) == 0) {
                }
            }
        }
        ret = 0;
    }
    return (ret);
}

int
ft_ping_routine(t_ft_ping *ft_ping) {
    print_introduction(ft_ping);

    (void)clock_gettime(CLOCK_MONOTONIC, &ft_ping->start_time);

    if (ft_ping->options.flood) {
        ft_ping->timer_spec.it_value.tv_sec  = 0;
        ft_ping->timer_spec.it_value.tv_nsec = 10 * 1e6;
    } else {
        ft_ping->timer_spec.it_value.tv_sec  = ft_ping->options_value.ms_interval_between_packets / 1000;
        ft_ping->timer_spec.it_value.tv_nsec = (ft_ping->options_value.ms_interval_between_packets % 1000) * 1e6;
    }

    if (ft_ping_main_loop(ft_ping) == -1) {
        return (-1);
    }

    print_outroduction(ft_ping);

    return (0);
}