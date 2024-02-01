/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/01 06:15:08 by plouvel           #+#    #+#             */
/*   Updated: 2024/02/01 06:16:49 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ICMP_UTILS_H
#define ICMP_UTILS_H

#include "ft_ping.h"

#define ICMP_PACKET_SIZE(packet_data_size) (sizeof(struct icmphdr) + (packet_data_size))

int      icmp_packet_decode(uint8_t *buffer, size_t buffer_size, t_packet_info *pi);
uint16_t icmp_checksum(void *payload, size_t payload_size);

#endif /* ICMP_UTILS_H*/