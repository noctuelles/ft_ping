/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/01 06:13:30 by plouvel           #+#    #+#             */
/*   Updated: 2024/02/14 22:57:40 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ICMP_ECHO_H
#define ICMP_ECHO_H

#include <netinet/ip_icmp.h>

#include "ft_ping.h"

void fill_icmp_echo_packet_header(struct icmphdr *icmp_echo_packet_header, uint16_t sequence_number,
                                  uint16_t sequence_id);
void fill_icmp_echo_request_packet_data(uint8_t *icmp_echo_packet_data, const t_data_pattern *data_pattern,
                                        size_t packet_data_size);

#endif /* ICMP_ECHO_H */