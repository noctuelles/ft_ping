/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ping.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/16 09:56:51 by plouvel           #+#    #+#             */
/*   Updated: 2024/01/19 16:51:35 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_PING_H
#define FT_PING_H

#include <stdint.h>

#include "translation.h"

#define TOGGLE_OPTION(flags, option) (flags ^= option)
#define HAS_OPTION(flags, option) (flags & option)

#define FT_PING_OPTION_COUNT 0x1
#define FT_PING_OPTION_PRELOAD 0x2
#define FT_PING_OPTION_HELP 0x4
#define FT_PING_OPTION_PACKET_DATA_SIZE 0x8
#define FT_PING_OPTION_TIMEOUT 0x10

#define DEFAULT_PACKET_DATA_SIZE 56

typedef struct s_ft_ping {
    uint32_t option_flags;
    struct {
        size_t stop_until_nbr_packets;
        size_t packet_data_size;
        size_t preload_nbr_packets;
        time_t timeout;
    } options_value;
    const char *target;
    t_socket    socket;
} t_ft_ping;

#endif  // FT_PING_H
