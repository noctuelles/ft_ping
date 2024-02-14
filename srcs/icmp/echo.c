/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/01 06:04:38 by plouvel           #+#    #+#             */
/*   Updated: 2024/02/14 23:28:49 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <netinet/ip_icmp.h>
#include <time.h>

#include "ft_ping.h"

/**
 * @brief Fill the header section of an ICMP Echo Request.
 * @note Does not fill the checksum field.
 *
 * @param icmp_echo_packet_header pointer to the beginning of the header packet section.
 * @param sequence_number the sequence number to be send with the packet.
 * @param sequence_id the sequence id to be send with the packet.
 */
void
fill_icmp_echo_packet_header(struct icmphdr *icmp_echo_packet_header, uint16_t sequence_number, uint16_t sequence_id) {
    icmp_echo_packet_header->type             = ICMP_ECHO;
    icmp_echo_packet_header->code             = 0;
    icmp_echo_packet_header->un.echo.id       = htons(sequence_id);
    icmp_echo_packet_header->un.echo.sequence = htons(sequence_number);
    icmp_echo_packet_header->checksum         = 0;
}

/**
 * @brief Fill the data section of an ICMP Echo Request.
 *
 * @param icmp_echo_packet_data pointer to the data section of the packet.
 * @param data_pattern a data pattern defined by the user through the --pattern option.
 * @param packet_data_size the size of the data section of the packet.
 */
void
fill_icmp_echo_request_packet_data(uint8_t *icmp_echo_packet_data, const t_data_pattern *data_pattern,
                                   size_t packet_data_size) {
    const size_t sizeof_timespec = sizeof(struct timespec);

    if (packet_data_size >= sizeof_timespec) {
        (void)clock_gettime(CLOCK_MONOTONIC, (struct timespec *)icmp_echo_packet_data);

        icmp_echo_packet_data += sizeof_timespec;
        packet_data_size -= sizeof_timespec;
    }

    for (size_t i = 0; i < packet_data_size; i++) {
        if (data_pattern->pattern_size > 0) {
            icmp_echo_packet_data[i] = data_pattern->pattern[i % data_pattern->pattern_size];
        } else {
            icmp_echo_packet_data[i] = i % 256;
        }
    }
}
