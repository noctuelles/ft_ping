/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/31 16:44:13 by plouvel           #+#    #+#             */
/*   Updated: 2024/01/31 16:44:37 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <netinet/ip_icmp.h>
#include <stdlib.h>
#include <sys/time.h>

double
compute_round_trip_time(struct icmp *icmp_packet) {
    struct timeval  currtime    = {0};
    struct timeval *packet_time = NULL;

    packet_time = (struct timeval *)icmp_packet->icmp_dun.id_data;
    (void)gettimeofday(&currtime, NULL);

    return ((double)(currtime.tv_sec - packet_time->tv_sec) * 1000.0 +
            (double)(currtime.tv_usec - packet_time->tv_usec) / 1000.0);
}