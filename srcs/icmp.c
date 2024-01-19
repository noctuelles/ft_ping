/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   icmp.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/17 16:15:44 by plouvel           #+#    #+#             */
/*   Updated: 2024/01/19 17:11:27 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "icmp.h"

#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#include "libft.h"

static void
fill_icmp_packet_data_custom(void *data, uint8_t data_pattern[16], size_t data_size) {
    for (size_t i = 0; i < data_size; i++) {
        if (data_pattern != NULL) {
            ((char *)data)[i] = data_pattern[i % 16];
        } else {
            ((char *)data)[i] = i % 256;
        }
    }
}

static void
fill_icmp_packet_data(void *data, uint8_t data_pattern[16], size_t data_size) {
    if (data_size >= sizeof(struct timeval)) {
        (void)gettimeofday((struct timeval *)data, NULL);

        fill_icmp_packet_data_custom(data + sizeof(struct timeval), data_pattern, data_size - sizeof(struct timeval));
    } else {
        fill_icmp_packet_data_custom(data, data_pattern, data_size);
    }
}

static void *
allocate_icmp_packet_payload(size_t data_size) {
    void *payload = NULL;

    if ((payload = malloc(sizeof(t_icmp_header) + data_size)) == NULL) {
        return (NULL);
    }

    ft_bzero(payload, sizeof(t_icmp_header) + data_size);

    return payload;
}

static uint16_t
icmp_payload_checksum(void *payload, size_t payload_size) {
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
 * Prepare an ICMP ECHO REQUEST packet by filling the ICMP header and the optional data.
 *
 * @param icmp_packet is the packet that will be filled.
 * @param data_pattern is the pattern that will be used to fill the optional data. If NULL, the optional data will be
 * filled with a sequence of bytes from 0 to 255 (modulo 256).
 * @param data_size is the size of the optional data. If >= sizeof(struct timeval), the optional data will contain the
 * current time and then the custom data.
 *
 * @return 0 on success, -1 on error. If an error occurs, the icmp_packet is not modified. The packet payload must be
 * freed by the caller.
 */
int
prepare_icmp_packet(t_icmp_packet *icmp_packet, uint8_t data_pattern[16], size_t data_size) {
    static size_t sequence_number = 0;

    if ((icmp_packet->payload = allocate_icmp_packet_payload(data_size)) == NULL) {
        return (-1);
    }

    icmp_packet->payload_size = sizeof(t_icmp_header) + data_size;
    icmp_packet->data         = icmp_packet->payload + sizeof(t_icmp_header);
    icmp_packet->data_size    = data_size;

    t_icmp_header *header = (t_icmp_header *)icmp_packet->payload;

    header->type            = ICMP_ECHO_REQUEST;
    header->code            = ICMP_CODE_ECHO_REQUEST;
    header->checksum        = 0;
    header->identifier      = (uint16_t)getpid();
    header->sequence_number = sequence_number++;

    fill_icmp_packet_data(icmp_packet->data, data_pattern, data_size);

    header->checksum = icmp_payload_checksum(icmp_packet->payload, icmp_packet->payload_size);

    return (0);
}