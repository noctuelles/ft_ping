/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/20 23:20:19 by plouvel           #+#    #+#             */
/*   Updated: 2024/02/14 23:47:11 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <time.h>

#include "ft_ping.h"
#include "icmp/common.h"
#include "icmp/print.h"
#include "utils/time.h"
#include "utils/wrapper.h"
#include "watchdog.h"

/**
 * @note Since the kernel sends us a copy of each incoming ICMP packets, we have to only keep the ICMP Error Messages
 * that concerns an ICMP Echo Request sent by our program.
 *
 * @param pi
 * @param host
 * @param id
 * @return true
 * @return false
 */
static bool
icmp_error_msg_not_for_us(const struct icmp *icmp_err_msg, const struct sockaddr_in *host, uint16_t id) {
    const struct ip   *orig_ip_packet   = NULL;
    const struct icmp *orig_icmp_packet = NULL;

    orig_ip_packet   = (const struct ip *)icmp_err_msg->icmp_dun.id_data;
    orig_icmp_packet = (const struct icmp *)(icmp_err_msg->icmp_dun.id_data + (orig_ip_packet->ip_hl << 2));

    if (orig_ip_packet->ip_dst.s_addr != host->sin_addr.s_addr)
        return (true);
    if (orig_ip_packet->ip_p != IPPROTO_ICMP)
        return (true);
    if (orig_icmp_packet->icmp_type != ICMP_ECHO && orig_icmp_packet->icmp_code != 0)
        return (true);
    if (ntohs(orig_icmp_packet->icmp_hun.ih_idseq.icd_id) != id)
        return (true);
    return (false);
}

static int
process_incoming_packet(t_ft_ping *ft_ping, const struct msghdr *msghdr, size_t bytes_recv) {
    char                   ip_str[INET_ADDRSTRLEN] = {0};
    bool                   duplicate               = false;
    int                    ret                     = 0;
    t_incoming_packet_info pi                      = {0};

    pi.from_sa = (struct sockaddr_in *)msghdr->msg_name;
    (void)inet_ntop(AF_INET, &pi.from_sa->sin_addr, ip_str, sizeof(ip_str));
    ret = icmp_packet_decode((uint8_t *)msghdr->msg_iov[0].iov_base, bytes_recv, &pi.ip, &pi.icmp);
    if (ret == -1) {
        fprintf(stderr, "packet too short (%lu bytes) from %s\n", bytes_recv, ip_str);
        return (0);
    }
    if (ret == 1) {
        fprintf(stderr, "checksum mismatch from %s\n", ip_str);
    }
    pi.icmp_size = ntohs(pi.ip->ip_len) - (pi.ip->ip_hl << 2);
    switch (pi.icmp->icmp_type) {
        /* Filters all ICMP Request (Echo, Timestamp, Address)  */
        case ICMP_ECHO:
        case ICMP_TIMESTAMP:
        case ICMP_ADDRESS:
            return (-1);
        case ICMP_ECHOREPLY:
            if (ntohs(pi.icmp->icmp_hun.ih_idseq.icd_id) != ft_ping->icmp.seq.id) {
                return (-1);
            }
            if (CHK_SEQ_NBR(ft_ping, ntohs(pi.icmp->icmp_hun.ih_idseq.icd_seq))) {
                duplicate = true;
            } else {
                SET_SEQ_NBR(ft_ping, ntohs(pi.icmp->icmp_hun.ih_idseq.icd_seq));
                duplicate = false;
            }
            if (duplicate) {
                ft_ping->stat.packet_duplicate++;
            } else {
                ft_ping->stat.packet_received++;
            }
            if (pi.icmp_size - ICMP_MINLEN >= (uint16_t)sizeof(struct timeval)) {
                ft_ping->stat.last_packet_rtt = compute_round_trip_time(pi.icmp);
                if (ft_ping->stat.last_packet_rtt < ft_ping->stat.min_packet_rtt) {
                    ft_ping->stat.min_packet_rtt = ft_ping->stat.last_packet_rtt;
                }
                if (ft_ping->stat.last_packet_rtt > ft_ping->stat.max_packet_rtt) {
                    ft_ping->stat.max_packet_rtt = ft_ping->stat.last_packet_rtt;
                }
                ft_ping->stat.avg_packet_rtt += ft_ping->stat.last_packet_rtt;
            }
            print_icmp_echo_reply(ft_ping, &pi, duplicate);
            break;
        default:
            if (icmp_error_msg_not_for_us(pi.icmp, &ft_ping->sockaddr.host, ft_ping->icmp.seq.id)) {
                return (-1);
            }
            print_icmp_default(&pi, ft_ping->options.verbose, ft_ping->options.numeric);
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
static ssize_t
ft_ping_main_loop(t_ft_ping *ft_ping) {
    struct msghdr      msg                       = {0};
    struct iovec       iovec[1]                  = {0};
    uint8_t            recv_buffer[IP_MAXPACKET] = {0};
    ssize_t            ret                       = 0;
    struct sockaddr_in from                      = {0};

    iovec[0].iov_base = recv_buffer;
    iovec[0].iov_len  = sizeof(recv_buffer);
    msg.msg_iov       = iovec;
    msg.msg_iovlen    = sizeof(iovec) / sizeof(struct iovec);
    msg.msg_name      = &from;
    msg.msg_namelen   = sizeof(from);

    while ((ret = watchdog(ft_ping)) == 1) {
        if (g_ping_state == RUNNING_RECV) {
            if ((ret = recvmsg_w(ft_ping->sock_fd, &msg, 0)) == -1) {
                return (-1);
            } else if (ret == RECVMSGW_TIMEOUT) {
                g_ping_state = FINISHING;
            } else if (ret > 0) {
                if (process_incoming_packet(ft_ping, &msg, ret) == 0) {
                }
            }
        }
        ret = 0;
    }
    return (ret);
}

int
ft_ping_routine(t_ft_ping *ft_ping) {
    ft_ping_print_intro(ft_ping);

    (void)clock_gettime(CLOCK_MONOTONIC, &ft_ping->start_time);

    if (ft_ping->options.flood) {
        ft_ping->timer_spec.it_value.tv_sec  = 0;
        ft_ping->timer_spec.it_value.tv_nsec = 10 * 1000000;
    } else {
        ft_ping->timer_spec.it_value.tv_sec  = ft_ping->options_value.ms_interval_between_packets / 1000;
        ft_ping->timer_spec.it_value.tv_nsec = (ft_ping->options_value.ms_interval_between_packets % 1000) * 1e6;
    }

    if (ft_ping_main_loop(ft_ping) == -1) {
        return (-1);
    }

    ft_ping_print_outro(ft_ping);

    return (0);
}