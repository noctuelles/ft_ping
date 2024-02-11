/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   output.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/20 22:10:17 by plouvel           #+#    #+#             */
/*   Updated: 2024/02/11 19:01:57 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef OUTPUT_H
#define OUTPUT_H

#include "ft_ping.h"

void print_introduction(const t_ft_ping *ft_ping);
void print_outroduction(const t_ft_ping *ft_ping);
void print_icmp_echo_reply(const t_ft_ping *ft_ping, t_incoming_packet_info *pi, bool duplicate);

#endif  // OUTPUT_H