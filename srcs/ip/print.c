/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/11 19:34:25 by plouvel           #+#    #+#             */
/*   Updated: 2024/02/14 23:02:24 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <arpa/inet.h>
#include <netinet/ip.h>
#include <stdio.h>

void
print_ip_header_hexdump(const struct ip* ip) {
    (void)printf("IP Hdr Dump:\n ");
    for (size_t i = 0; i < sizeof(*ip); i++) {
        printf("%02x%s", *((uint8_t*)ip + i), i % 2 != 0 ? " " : "");
    }
    (void)printf("\n");
}

void
print_ip_header(const struct ip* ip) {
    (void)printf("Vr HL TOS  Len   ID Flg  off TTL Pro  cks\tSrc\tDst\n");
    (void)printf(" %1x  %1x  %02x %04x %04x  %02x %04x  %02x  %02x %04x %s  %s\n", ip->ip_v, ip->ip_hl, ip->ip_tos,
                 ntohs(ip->ip_len), ntohs(ip->ip_id), 0, ntohs(ip->ip_off), ip->ip_ttl, ip->ip_p, ntohs(ip->ip_sum),
                 inet_ntoa(ip->ip_src), inet_ntoa(ip->ip_src));
}