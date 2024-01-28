/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   icmp.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/17 15:11:51 by plouvel           #+#    #+#             */
/*   Updated: 2024/01/27 15:26:01 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ICMP_H
#define ICMP_H

#include <netinet/ip_icmp.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "ft_ping.h"

#define ICMP_ECHO_PACKET_SIZE(packet_data_size) (sizeof(struct icmphdr) + (packet_data_size))

void         fill_icmp_echo_packet_header(struct icmphdr *icmp_echo_packet_header, uint16_t sequence_number,
                                          uint16_t sequence_id);
struct icmp *allocate_icmp_echo_packet(size_t packet_data_size);
void         fill_icmp_echo_packet_data(uint8_t *icmp_echo_packet_data, const t_data_pattern *data_pattern,
                                        size_t packet_data_size);
int          icmp_packet_decode(uint8_t *buffer, size_t buffer_size, struct ip **rip, struct icmp **ricmp);

#endif