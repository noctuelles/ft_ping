/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   default.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/11 11:47:32 by plouvel           #+#    #+#             */
/*   Updated: 2024/02/11 11:48:08 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ICMP_DEFAULT_H
#define ICMP_DEFAULT_H

#include "ft_ping.h"

void print_icmp_default(const t_incoming_packet_info *pi, bool verbose, bool numeric_only);

#endif /* ICMP_DEFAULT_H */