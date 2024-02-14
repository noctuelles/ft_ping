/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/14 23:03:38 by plouvel           #+#    #+#             */
/*   Updated: 2024/02/14 23:12:50 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_PING_PRINT_H
#define FT_PING_PRINT_H

#include "ft_ping.h"

void print_icmp_default(const t_incoming_packet_info *pi, bool verbose, bool numeric_only);
void print_icmp_echo_reply(const t_ft_ping *ft_ping, const t_incoming_packet_info *pi, bool duplicate);

#endif  // FT_PING_PRINT_H
