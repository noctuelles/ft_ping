/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   icmp.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/17 16:15:44 by plouvel           #+#    #+#             */
/*   Updated: 2024/01/22 00:50:02 by plouvel          ###   ########.fr       */
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

static void
fill_icmp_echo_packet_data(uint8_t *icmp_echo_packet_data, const t_data_pattern *data_pattern, size_t data_size) {
    if (data_size >= sizeof(struct timeval)) {
        (void)gettimeofday((struct timeval *)icmp_echo_packet_data, NULL);

        icmp_echo_packet_data += sizeof(struct timeval);
        data_size -= sizeof(struct timeval);
    }

    for (size_t i = 0; i < data_size; i++) {
        if (data_pattern != NULL) {
            icmp_echo_packet_data[i] = data_pattern->pattern[i % data_pattern->pattern_size];
        } else {
            icmp_echo_packet_data[i] = i % 256;
        }
    }
}

static void *
allocate_icmp_packet(size_t data_size) {
    struct icmp *icmp_packet;

    if ((icmp_packet = malloc(sizeof(struct icmphdr) + data_size) == NULL)) {
        return (NULL);
    }

    return icmp_packet;
}

/**
 * Prepare an ICMP ECHO REQUEST packet by filling the ICMP header and the optional data.
 *
 * @param data_pattern is the pattern that will be used to fill the optional data. If NULL, the optional data will be
 * filled with a sequence of bytes from 0 to 255.
 * @param seq is the sequence number of the packet.
 * @param data_size is the size of the optional data. If >= sizeof(struct timeval), the optional data will contain the
 * current time and then the custom data.
 *
 * @return the allocated icmp echo packet or NULL on failure.
 */
struct icmp *
prepare_icmp_echo_packet(const t_data_pattern *data_pattern, uint16_t seq, size_t data_size) {
    struct icmp    *icmp_echo_packet = NULL;
    static uint16_t seq              = 0;

    if ((icmp_echo_packet = malloc(sizeof(struct icmphdr) + data_size) == NULL)) {
        return (NULL);
    }

    icmp_echo_packet->icmp_type                 = ICMP_ECHO;
    icmp_echo_packet->icmp_code                 = 0;
    icmp_echo_packet->icmp_cksum                = 0;
    icmp_echo_packet->icmp_hun.ih_idseq.icd_id  = getpid() & 0xFFFFu;
    icmp_echo_packet->icmp_hun.ih_idseq.icd_seq = seq;

    fill_icmp_echo_packet_data(icmp_echo_packet->icmp_dun.id_data, data_pattern, data_size);

    icmp_echo_packet->icmp_cksum = internet_checksum(icmp_echo_packet, sizeof(struct icmphdr) + data_size);

    return (icmp_echo_packet);
}
