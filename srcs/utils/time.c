/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/31 16:44:13 by plouvel           #+#    #+#             */
/*   Updated: 2024/02/06 05:07:00 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <netinet/ip_icmp.h>
#include <stdlib.h>
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
    struct timeval  currtime    = {0};
    struct timeval *packet_time = NULL;

    packet_time = (struct timeval *)icmp_packet->icmp_dun.id_data;
    (void)gettimeofday(&currtime, NULL);

    return ((double)(currtime.tv_sec - packet_time->tv_sec) * 1000.0 +
            (double)(currtime.tv_usec - packet_time->tv_usec) / 1000.0);
}

/**
 * @brief Get a timeval structure from a number of seconds.
 *
 * @param seconds the number of seconds to convert to timeval (e.g 1.5s)
 * @return struct timeval the timeval object
 */
struct timeval
get_timeval_from_secs(double seconds) {
    struct timeval tv = {0};

    tv.tv_sec  = (time_t)seconds;
    tv.tv_usec = (suseconds_t)((seconds - (double)tv.tv_sec) * 1000000.0);

    return (tv);
}