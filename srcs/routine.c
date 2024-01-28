/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/20 23:20:19 by plouvel           #+#    #+#             */
/*   Updated: 2024/01/28 10:32:07 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "checksum.h"
#include "ft_ping.h"
#include "icmp.h"
#include "libft.h"
#include "output.h"

static ssize_t
sendto_w(const t_ft_ping *ft_ping, const struct icmp *icmp_packet, size_t packet_size) {
    ssize_t ret;

    if ((ret = sendto(ft_ping->hostsock_fd, icmp_packet, packet_size, MSG_NOSIGNAL,
                      (struct sockaddr *)&ft_ping->hostsock_addr, ft_ping->sock_len)) == -1) {
        fprintf(stderr, "ft_ping: sendto: %s\n", strerror(errno));
    }

    return ret;
}

/**
 * @brief
 * @note the icmp_echo_packet only need to be allocated once, since the packet size doesn't change during runtime.
 *
 * @param signum
 */
static int
send_packet(t_ft_ping *ft_ping, struct icmp *icmp_echo_packet) {
    fill_icmp_echo_packet_header((struct icmphdr *)icmp_echo_packet, ft_ping->seq.nbr++, ft_ping->seq.id);
    fill_icmp_echo_packet_data(icmp_echo_packet->icmp_dun.id_data, &ft_ping->options_value.packet_data_pattern,
                               ft_ping->options_value.packet_data_size);
    icmp_echo_packet->icmp_cksum =
        icmp_checksum(icmp_echo_packet, ICMP_ECHO_PACKET_SIZE(ft_ping->options_value.packet_data_size));
    if (sendto_w(ft_ping, icmp_echo_packet, ICMP_ECHO_PACKET_SIZE(ft_ping->options_value.packet_data_size)) == -1) {
        return (-1);
    }

    return (0);
}

void
icmp_echo_handler(const struct ip *ip, const struct icmp *imcp) {}

void
on_recv(t_ft_ping *ft_ping, uint8_t *buffer, size_t buffer_size) {
    struct ip   *ip     = NULL;
    struct icmp *icmp   = NULL;
    int          result = 0;
    bool         dup    = false;

    result = icmp_packet_decode(buffer, buffer_size, &ip, &icmp);
    if (result == -1) {
        fprintf(stderr, "packet too short (%lu bytes) from %s\n", buffer_size, ft_ping->p_inet_addr);
        return;
    }
    if (result == 1) {
        fprintf(stderr, "checksum mismatch from %s\n", ft_ping->p_inet_addr);
    }
    if (icmp->icmp_type == ICMP_ECHOREPLY) {
        if (ntohs(icmp->icmp_hun.ih_idseq.icd_id) != ft_ping->seq.id) {
            return;
        }

        if (CHK_SEQ_NBR(ft_ping, ntohs(icmp->icmp_hun.ih_idseq.icd_seq))) {
            dup = true;
        } else {
            SET_SEQ_NBR(ft_ping, ntohs(icmp->icmp_hun.ih_idseq.icd_seq));
            dup = false;
        }

        print_echo(ft_ping, ip, icmp, dup);
    } else {
    }
}

void
echo() {}

void
ping_routine(t_ft_ping *ft_ping) {
    struct icmp  *icmp_echo_packet;
    ssize_t       bytes_received = 0;
    struct msghdr msg            = {0};
    uint8_t       recv_buffer[IP_MAXPACKET];

    if ((icmp_echo_packet = allocate_icmp_echo_packet(ft_ping->options_value.packet_data_size)) == NULL) {
        return;
    }

    msg.msg_iov     = (struct iovec[]){(struct iovec){.iov_base = recv_buffer, .iov_len = IP_MAXPACKET}};
    msg.msg_iovlen  = 1;
    msg.msg_name    = &ft_ping->recvsock_addr;
    msg.msg_namelen = ft_ping->sock_len;

    print_introduction(ft_ping);

    for (size_t i = 0; i < ft_ping->options_value.preload_nbr_packets; i++) {
        if (send_packet(ft_ping, icmp_echo_packet) == -1) {
            return;
        }
    }

    while (g_ping_state != END_PROGRAM) {
        g_ping_state = RECV_MSG;

        alarm(ft_ping->options_value.interval_between_packets);

        bytes_received = recvmsg(ft_ping->hostsock_fd, &msg, 0);

        if (bytes_received == -1) {
            if (errno == EINTR) {
                if (g_ping_state == SEND_MSG) {
                    if (send_packet(ft_ping, icmp_echo_packet) == -1) {
                        return;
                    }

                    (void)alarm(ft_ping->options_value.interval_between_packets);
                }
            } else {
                fprintf(stderr, "ft_ping: recvmsg: %s\n", strerror(errno));
            }
        } else {
            on_recv(ft_ping, recv_buffer, bytes_received);
        }
    }
}