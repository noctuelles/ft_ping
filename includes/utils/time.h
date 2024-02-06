/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/01 06:20:52 by plouvel           #+#    #+#             */
/*   Updated: 2024/02/06 05:00:58 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_TIME_H
#define UTILS_TIME_H

#include <netinet/ip_icmp.h>

double         compute_round_trip_time(struct icmp *icmp_packet);
struct timeval get_timeval_from_secs(double seconds);

#endif /* UTILS_TIME_H */