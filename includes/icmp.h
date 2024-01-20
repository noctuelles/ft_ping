/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   icmp.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/17 15:11:51 by plouvel           #+#    #+#             */
/*   Updated: 2024/01/20 14:16:58 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ICMP_H
#define ICMP_H

#include <stddef.h>
#include <stdint.h>

#include "ft_ping.h"

#define ICMP_ECHO_REQUEST 8
#define ICMP_CODE_ECHO_REQUEST 0

/**
 * type is the type of the ICMP message.
 * code is the code of the ICMP message.
 * checksum is the 16-bit one's complement of the one's complement sum of the ICMP message.
 * identifier on this implementation is equal to the process id (pid).
 * sequence number is incremented for each packet sent.
 */
typedef struct s_icmp_header {
    uint8_t  type;
    uint8_t  code;
    uint16_t checksum;
    uint16_t identifier;
    uint16_t sequence_number;
} t_icmp_header;

/**
 * payload is the data that will be sent over the network. It points to a memory region that contains the ICMP header,
 * and the following optional data.
 * payload_size is the size of the payload.
 * data is pointing to a memeory region within payload, that contains the optional data.
 */
typedef struct s_icmp_packet {
    void  *payload;
    size_t payload_size;
    void  *data;
    size_t data_size;
} t_icmp_packet;

int prepare_icmp_packet(t_icmp_packet *icmp_packet, t_data_pattern *data_pattern, size_t data_size);

#endif