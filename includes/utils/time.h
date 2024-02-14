/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/01 06:20:52 by plouvel           #+#    #+#             */
/*   Updated: 2024/02/14 23:10:17 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_TIME_H
#define UTILS_TIME_H

#include <netinet/ip_icmp.h>

double compute_round_trip_time(struct icmp *icmp_packet);

#endif /* UTILS_TIME_H */