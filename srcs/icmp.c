/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   icmp.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/17 16:15:44 by plouvel           #+#    #+#             */
/*   Updated: 2024/01/27 15:25:48 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "icmp.h"

#include <netinet/ip_icmp.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#include "checksum.h"
#include "ft_ping.h"
#include "libft.h"

void
fill_icmp_echo_packet_data(uint8_t *icmp_echo_packet_data, const t_data_pattern *data_pattern,
                           size_t packet_data_size) {
    if (packet_data_size >= sizeof(struct timeval)) {
        (void)gettimeofday((struct timeval *)icmp_echo_packet_data, NULL);

        icmp_echo_packet_data += sizeof(struct timeval);
        packet_data_size -= sizeof(struct timeval);
    }

    for (size_t i = 0; i < packet_data_size; i++) {
        if (data_pattern->pattern_size > 0) {
            icmp_echo_packet_data[i] = data_pattern->pattern[i % data_pattern->pattern_size];
        } else {
            icmp_echo_packet_data[i] = i % 256;
        }
    }
}

/**
 * @brief This function decode an incoming datagram returned by the kernel. If successfull, rip and ricmp value are
 * updated to the start of the IP Header and the ICMP Header respectively.
 *
 * @param buffer
 * @param buffer_size
 * @param rip value-return argument.
 * @param ricmp value-return argument.
 * @return int 0: the function sucessfully parsed the datagram. -1 : the packet is too short, 1 : the checksum doesn't
 * match.
 */
int
icmp_packet_decode(uint8_t *buffer, size_t buffer_size, struct ip **rip, struct icmp **ricmp) {
    struct ip   *ip               = NULL;
    struct icmp *icmp             = NULL;
    size_t       ip_header_len    = 0;
    size_t       icmp_payload_len = 0;
    uint16_t     saved_cksum      = 0;

    ip               = (struct ip *)buffer;
    ip_header_len    = ip->ip_hl << 2U;
    icmp_payload_len = ntohs(ip->ip_len) - ip_header_len;
    if (buffer_size < ip_header_len + ICMP_MINLEN) {
        return (-1);
    }
    icmp             = (struct icmp *)(buffer + ip_header_len);
    saved_cksum      = icmp->icmp_cksum;
    icmp->icmp_cksum = 0;
    icmp->icmp_cksum = icmp_checksum(icmp, icmp_payload_len);
    if (icmp->icmp_cksum != saved_cksum) {
        return (1);
    }
    icmp->icmp_cksum = saved_cksum;
    *rip             = ip;
    *ricmp           = icmp;
    return (0);
}

void
fill_icmp_echo_packet_header(struct icmphdr *icmp_echo_packet_header, uint16_t sequence_number, uint16_t sequence_id) {
    icmp_echo_packet_header->type             = ICMP_ECHO;
    icmp_echo_packet_header->code             = 0;
    icmp_echo_packet_header->un.echo.id       = htons(sequence_id);
    icmp_echo_packet_header->un.echo.sequence = htons(sequence_number);
    icmp_echo_packet_header->checksum         = 0;
}

struct icmp *
allocate_icmp_echo_packet(size_t packet_data_size) {
    struct icmp *icmp_echo_packet;

    if ((icmp_echo_packet = malloc(ICMP_ECHO_PACKET_SIZE(packet_data_size))) == NULL) {
        return (NULL);
    }

    return (icmp_echo_packet);
}
