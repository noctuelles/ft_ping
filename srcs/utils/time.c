/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/31 16:44:13 by plouvel           #+#    #+#             */
/*   Updated: 2024/02/14 23:59:30 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <netinet/ip_icmp.h>
#include <sys/time.h>
#include <time.h>

/**
 * @brief Return the RTT of an ICMP packet.
 *
 * @param icmp_packet pointer to the ICMP packet.
 * @return double RTT
 */
double
compute_round_trip_time(struct icmp *icmp_packet) {
    struct timespec  currtime    = {0};
    struct timespec *packet_time = NULL;

    packet_time = (struct timespec *)icmp_packet->icmp_dun.id_data;
    (void)clock_gettime(CLOCK_MONOTONIC, &currtime);

    return ((double)(currtime.tv_sec - packet_time->tv_sec) * 1000.0 +
            (double)(currtime.tv_nsec - packet_time->tv_nsec) / 1000000.0);
}