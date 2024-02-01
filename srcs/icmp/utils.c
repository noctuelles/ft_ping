/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/17 16:15:44 by plouvel           #+#    #+#             */
/*   Updated: 2024/02/01 14:45:40 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <netinet/ip_icmp.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#include "ft_ping.h"
#include "libft.h"
#include "utils/wrapper.h"

/**
 * @brief compute the internet checksum of a payload for an ICMPv4 packet (ICMPv6 is computed by the kernel)
 * @note see https://tools.ietf.org/html/rfc1071
 *
 * @param payload pointer to the payload
 * @param payload_size size of the payload
 * @return uint16_t the checksum
 */
uint16_t
icmp_checksum(void *payload, size_t payload_size) {
    uint32_t  checksum = 0;
    uint16_t *ptr      = (uint16_t *)payload;

    payload_size /= sizeof(uint16_t);

    for (size_t i = 0; i < payload_size; i++) {
        checksum += ptr[i];
        checksum = (checksum & 0xffff) + (checksum >> 16);
    }

    return (uint16_t)(~checksum & 0xffff);
}

/**
 * @brief Decode an incoming IP datagram returned by the kernel. If successfull, the packet info pi is
 * updated.
 *
 * @param buffer pointer to buffer returned by the kernel.
 * @param buffer_size buffer size returned by the kernel.
 * @param pi pointer to the packet info.
 * @return int 0: the function sucessfully parsed the datagram. -1 : the packet is too short (fatal error), 1 : the
 * checksum doesn't match.
 */
int
icmp_packet_decode(uint8_t *buffer, size_t buffer_size, t_packet_info *pi) {
    struct ip   *ip               = NULL;
    struct icmp *icmp             = NULL;
    size_t       ip_header_len    = 0;
    size_t       icmp_payload_len = 0;
    uint16_t     saved_cksum      = 0;

    ip                   = (struct ip *)buffer;
    pi->ip               = ip;
    ip_header_len        = ip->ip_hl << 2U;
    icmp_payload_len     = ntohs(ip->ip_len) - ip_header_len;
    pi->icmp_payload_len = icmp_payload_len;
    if (buffer_size <
        ip_header_len + ICMP_MINLEN) { /* In case there's not even an ICMP Header included in ip payload */
        return (-1);
    }
    icmp             = (struct icmp *)(buffer + ip_header_len);
    pi->icmp         = icmp;
    saved_cksum      = icmp->icmp_cksum;
    icmp->icmp_cksum = 0;
    icmp->icmp_cksum = icmp_checksum(icmp, icmp_payload_len);
    if (icmp->icmp_cksum != saved_cksum) {
        return (1);
    }
    icmp->icmp_cksum = saved_cksum;
    return (0);
}
