/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/01 06:15:08 by plouvel           #+#    #+#             */
/*   Updated: 2024/02/11 11:40:05 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ICMP_UTILS_H
#define ICMP_UTILS_H

#include "ft_ping.h"

#define ICMP_PACKET_SIZE(packet_data_size) (sizeof(struct icmphdr) + (packet_data_size))

int      icmp_packet_decode(uint8_t *buffer, size_t buffer_size, t_incoming_packet_info *pi);
uint16_t icmp_checksum(void *payload, size_t payload_size);

void print_icmp_type_code_str(uint8_t type, uint8_t code);

#endif /* ICMP_UTILS_H*/