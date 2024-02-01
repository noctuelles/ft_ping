/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/20 23:20:19 by plouvel           #+#    #+#             */
/*   Updated: 2024/02/01 14:47:17 by plouvel          ###   ########.fr       */
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
#include "utils/wrapper.h"

static void
recv_packet(t_ft_ping *ft_ping, uint8_t *buffer, size_t buffer_size) {
    t_packet_info pi     = {0};
    int           result = 0;

    result = icmp_packet_decode(buffer, buffer_size, &pi);
    if (result == -1) {
        fprintf(stderr, "packet too short (%lu bytes) from %s\n", buffer_size, ft_ping->sockaddr.sender_presentation);
        return;
    }
    if (result == 1) {
        fprintf(stderr, "checksum mismatch from %s\n", ft_ping->sockaddr.sender_presentation);
    }
    if (pi.icmp->icmp_type == ICMP_ECHOREPLY) {
        if (ntohs(pi.icmp->icmp_hun.ih_idseq.icd_id) != ft_ping->icmp.seq.id) {
            return;
        }

        if (CHK_SEQ_NBR(ft_ping, ntohs(pi.icmp->icmp_hun.ih_idseq.icd_seq))) {
            pi.duplicate = true;
        } else {
            SET_SEQ_NBR(ft_ping, ntohs(pi.icmp->icmp_hun.ih_idseq.icd_seq));
            pi.duplicate = false;
        }

        ft_ping_on_recv_update_stat(&ft_ping->stat, &pi);

        print_echo(ft_ping, &pi);
    }
}

static int
ft_ping_main_loop(t_ft_ping *ft_ping) {
    struct msghdr msg                       = {0};
    struct iovec  iovec[1]                  = {0};
    uint8_t       recv_buffer[IP_MAXPACKET] = {0};
    ssize_t       recv_buffer_size          = 0;

    iovec[0].iov_base = recv_buffer;
    iovec[0].iov_len  = sizeof(recv_buffer);
    msg.msg_iov       = iovec;
    msg.msg_iovlen    = sizeof(iovec) / sizeof(struct iovec);
    msg.msg_name      = &ft_ping->sockaddr.sender;
    msg.msg_namelen   = ft_ping->sockaddr.len;

    g_ping_state = RECV_MSG;

    (void)timer_settime(ft_ping->timer.id, 0, &ft_ping->timer.value, NULL);

    while (g_ping_state == RECV_MSG) {
        if ((recv_buffer_size = recvmsg_w(ft_ping->sock_fd, &msg, 0)) == -1) {
            if (errno != EINTR) {
                return (-1);
            } else {
                if (g_ping_state == SEND_MSG) {
                    if (icmp_echo_send_packet(ft_ping) == -1) {
                        return (-1);
                    }
                    g_ping_state = RECV_MSG;
                }
            }
        } else {
            (void)inet_ntop(AF_INET, &((struct sockaddr_in *)&ft_ping->sockaddr.sender)->sin_addr,
                            ft_ping->sockaddr.sender_presentation, sizeof(ft_ping->sockaddr.sender_presentation));

            recv_packet(ft_ping, recv_buffer, (size_t)recv_buffer_size);
        }
    }
    return (0);
}

int
ft_ping_routine(t_ft_ping *ft_ping) {
    print_introduction(ft_ping);

    /* The default preload value is one, even if the user didn't mentioned any --preload option. */
    for (size_t i = 0; i < ft_ping->options_value.preload_nbr_packets; i++) {
        if (icmp_echo_send_packet(ft_ping) == -1) {
            return (-1);
        }
    }

    if (ft_ping_main_loop(ft_ping) == -1) {
        return (-1);
    }

    print_outroduction(ft_ping);

    return (0);
}