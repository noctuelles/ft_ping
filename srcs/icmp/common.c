/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   common.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/17 16:15:44 by plouvel           #+#    #+#             */
/*   Updated: 2024/02/14 23:25:08 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <netinet/ip_icmp.h>
#include <stdlib.h>

/**
 * @brief compute the internet checksum of a payload for an ICMPv4 packet (ICMPv6 is computed by the kernel)
 * @note see https://tools.ietf.org/html/rfc1071
 *
 * @param payload pointer to the payload
 * @param payload_len size of the payload
 * @return uint16_t the checksum
 */
uint16_t
icmp_checksum(void *payload, size_t payload_len) {
    uint32_t  checksum   = 0;
    uint16_t *ptr        = (uint16_t *)payload;
    int32_t   bytes_left = (int32_t)payload_len;

    while (bytes_left > 1) {
        checksum += *ptr++;
        checksum = (checksum & 0xffff) + (checksum >> 16);
        bytes_left -= 2;
    }

    if (bytes_left == 1) {
        checksum += *(uint8_t *)ptr;
        checksum = (checksum & 0xffff) + (checksum >> 16);
    }

    return (uint16_t)(~checksum & 0xffff);
}

/**
 * @brief Decode an incoming raw buffer, and put the IP and ICMP header in the pointers provided.
 * @param buffer pointer to the buffer
 * @param buffer_size size of the buffer
 * @param p_ip pointer to the pointer that will receive the IP header
 * @param p_icmp pointer to the pointer that will receive the ICMP header
 * @return int 0 if the packet is valid, 1 if the checksum is invalid, -1 if the buffer is too small.
 */
int
icmp_packet_decode(const uint8_t *buffer, size_t buffer_size, struct ip **p_ip, struct icmp **p_icmp) {
    struct ip   *ip            = NULL;
    struct icmp *icmp          = NULL;
    size_t       ip_header_len = 0;
    uint16_t     saved_cksum   = 0;

    ip            = (struct ip *)buffer;
    *p_ip         = ip;
    ip_header_len = ip->ip_hl << 2U;
    if (buffer_size < ip_header_len + ICMP_MINLEN) {
        return (-1);
    }
    icmp             = (struct icmp *)(buffer + ip_header_len);
    *p_icmp          = icmp;
    saved_cksum      = icmp->icmp_cksum;
    icmp->icmp_cksum = 0;
    icmp->icmp_cksum = icmp_checksum(icmp, ntohs(ip->ip_len) - ip_header_len);
    if (icmp->icmp_cksum != saved_cksum) {
        return (1);
    }
    icmp->icmp_cksum = saved_cksum;
    return (0);
}
