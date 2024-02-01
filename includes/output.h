/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   output.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/20 22:10:17 by plouvel           #+#    #+#             */
/*   Updated: 2024/02/01 06:23:34 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef OUTPUT_H
#define OUTPUT_H

#include "ft_ping.h"

void print_introduction(const t_ft_ping *ft_ping);
void print_outroduction(const t_ft_ping *ft_ping);
void print_echo(const t_ft_ping *ft_ping, t_packet_info *pi);

#endif  // OUTPUT_H