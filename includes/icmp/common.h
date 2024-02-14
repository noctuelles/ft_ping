/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   common.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/01 06:15:08 by plouvel           #+#    #+#             */
/*   Updated: 2024/02/14 23:14:48 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ICMP_COMMON_H
#define ICMP_COMMON_H

#include "ft_ping.h"

int      icmp_packet_decode(const uint8_t *buffer, size_t buffer_size, struct ip **p_ip, struct icmp **p_icmp);
uint16_t icmp_checksum(void *payload, size_t payload_size);

#endif /* ICMP_COMMON_H*/