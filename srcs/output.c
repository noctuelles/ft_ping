/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   output.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/20 22:05:14 by plouvel           #+#    #+#             */
/*   Updated: 2024/01/30 05:10:22 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <stdio.h>

#include "ft_args_parser.h"
#include "ft_ping.h"
#include "libft.h"

void
print_introduction(const t_ft_ping *ft_ping) {
    (void)printf("PING %s (%s): %lu data bytes", ft_ping->node, ft_ping->p_inet_addr,
                 ft_ping->options_value.packet_data_size);
    if (HAS_OPT(ft_ping, OPT_VERBOSE)) {
        (void)printf(", id %#04x = %u", ft_ping->seq.id, ft_ping->seq.id);
    }
    (void)printf("\n");
}

void
print_echo(const t_ft_ping *ft_ping, const struct ip *ip, const struct icmp *icmp, bool dup) {
    if (HAS_OPT(ft_ping, OPT_FLOOD)) {
        ft_putchar('\b');
        return;
    }

    printf("%u bytes from %s: icmp_seq=%u ttl=%u", (uint16_t)(ntohs(ip->ip_len) - (ip->ip_hl << 2U)),
           ft_ping->p_inet_addr, ntohs(icmp->icmp_hun.ih_idseq.icd_seq), ip->ip_ttl);
    if (dup) {
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