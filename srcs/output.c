/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   output.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/20 22:05:14 by plouvel           #+#    #+#             */
/*   Updated: 2024/02/01 06:23:58 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <stdio.h>
#include <sys/time.h>

#include "ft_args_parser.h"
#include "ft_ping.h"
#include "libft.h"

void
print_introduction(const t_ft_ping *ft_ping) {
    (void)printf("PING %s (%s): %lu data bytes", ft_ping->node, ft_ping->sockaddr.host_presentation,
                 ft_ping->options_value.packet_data_size);
    if (HAS_OPT(ft_ping, OPT_VERBOSE)) {
        (void)printf(", id %#04x = %u", ft_ping->icmp.seq.id, ft_ping->icmp.seq.id);
    }
    (void)printf("\n");
}

void
print_outroduction(const t_ft_ping *ft_ping) {
    (void)printf("--- %s ping statistics ---\n", ft_ping->node);

    (void)printf("%lu packets transmitted, %lu received", ft_ping->stat.packet_sent, ft_ping->stat.packet_received);
    if (ft_ping->stat.packet_duplicate != 0) {
        (void)printf(", +%lu duplicates", ft_ping->stat.packet_duplicate);
    }
    (void)printf(", %lu%% packet loss",
                 (ft_ping->stat.packet_sent - ft_ping->stat.packet_received) * 100 / ft_ping->stat.packet_sent);
    (void)printf("\n");

    if (ft_ping->stat.last_packet_rtt != 0.0) {
        (void)printf("round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\n", ft_ping->stat.min_packet_rtt,
                     ft_ping->stat.avg_packet_rtt / (double)ft_ping->stat.packet_received, ft_ping->stat.max_packet_rtt,
                     0.0f);
    }
}

void
print_echo(const t_ft_ping *ft_ping, const t_packet_info *pi) {
    if (HAS_OPT(ft_ping, OPT_FLOOD)) {
        ft_putchar('\b');
        return;
    }
    printf("%u bytes from %s: icmp_seq=%u ttl=%u", pi->icmp_payload_len, ft_ping->sockaddr.sender_presentation,
           ntohs(pi->icmp->icmp_hun.ih_idseq.icd_seq), pi->ip->ip_ttl);
    if (pi->icmp_payload_len - ICMP_MINLEN >= (uint16_t)sizeof(struct timeval)) {
        printf(" time=%.3f ms", ft_ping->stat.last_packet_rtt);
    }
    if (pi->duplicate) {
        printf(" (DUP!)");
    }
    printf("\n");
}

void
dump_icmp(const struct icmp *icmp_packet) {
    (void)printf("ICMP: type %u, code %u, checksum %u, id %#04x, seq %#04x\n", icmp_packet->icmp_type,
                 icmp_packet->icmp_code, icmp_packet->icmp_cksum, icmp_packet->icmp_hun.ih_idseq.icd_id,
                 icmp_packet->icmp_hun.ih_idseq.icd_seq);
}

void
dump_ip_header_hex(const struct ip *ip) {
    size_t hdr_size = ip->ip_hl * 4;

    ft_putstr("IP Hdr Dump:\n");

    for (size_t i = 0; i < hdr_size; i += 2) {
        (void)printf(" %x", ((uint16_t *)ip)[i]);
    }

    ft_putchar('\n');
}

void
dump_ip_header(const struct ip *ip) {
    (void)ip;

    ft_putstr("Vr HL TOS  Len   ID Flg  off TTL Pro  cks      Src	Dst	Data\n");
}