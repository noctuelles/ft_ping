/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ping.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/16 09:56:51 by plouvel           #+#    #+#             */
/*   Updated: 2024/01/20 15:01:52 by plouvel          ###   ########.fr       */
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

typedef struct s_data_pattern {
    uint8_t *pattern;
    size_t   pattern_size;
} t_data_pattern;

typedef struct s_ft_ping {
    uint32_t option_flags;
    struct {
        size_t         stop_until_nbr_packets;
        size_t         packet_data_size;
        size_t         packet_time_to_live;
        size_t         preload_nbr_packets;
        time_t         timeout;
        t_data_pattern data_pattern;
    } options_value;
    const char *target;
    t_socket    socket;
} t_ft_ping;

extern t_ft_ping g_ft_ping;

#endif  // FT_PING_H
