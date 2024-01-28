/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   output.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/20 22:10:17 by plouvel           #+#    #+#             */
/*   Updated: 2024/01/27 15:06:28 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef OUTPUT_H
#define OUTPUT_H

#include "ft_ping.h"

void print_introduction(const t_ft_ping *ft_ping);

void print_echo(const t_ft_ping *ft_ping, const struct ip *ip, const struct icmp *icmp, bool dup);

#endif  // OUTPUT_H