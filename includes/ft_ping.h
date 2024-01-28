/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ping.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/16 09:56:51 by plouvel           #+#    #+#             */
/*   Updated: 2024/01/28 11:07:13 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_PING_H
#define FT_PING_H

#include <stdint.h>

#include "translation.h"

#define TOGGLE_OPT(ft_ping, option) ((ft_ping)->option_flags ^= option)
#define HAS_OPT(ft_ping, option) ((ft_ping)->option_flags & option)

#define CHK_SEQ_NBR(ft_ping, nbr) ((ft_ping)->seq_nbr_chk[nbr / 8] & (1U << nbr % 8))
#define SET_SEQ_NBR(ft_ping, nbr) ((ft_ping)->seq_nbr_chk[nbr / 8] ^= (1U << nbr % 8))

#define U16_PID() (getpid() & 0xFFFFU)
#define PATTERN_MAX_SIZE 16

#define OPT_COUNT 0x1
#define OPT_DEBUG 0x2
#define OPT_INTERVAL 0x4
#define OPT_NUMERIC 0x8
#define OPT_IGNORE_ROUTING 0x10
#define OPT_TIME_TO_LIVE 0x20
#define OPT_TYPE_OF_SERVICE 0x40
#define OPT_VERBOSE 0x80
#define OPT_TIMEOUT 0x100
#define OPT_LINGER 0x200

#define OPT_FLOOD 0x400
#define OPT_IP_TIMESTAMP 0x800
#define OPT_PRELOAD 0x1000
#define OPT_PATTERN 0x2000
#define OPT_QUIET 0x4000
#define OPT_ROUTE 0x8000
#define OPT_SIZE 0x10000

#define DEFAULT_PACKET_DATA_SIZE 56
#define DEFAULT_PRELOAD_NBR_PACKETS 1
#define DEFAULT_INTERVAL_BETWEEN_PACKET 1
#define SEQ_NBR_CHK_SIZE ((UINT16_MAX + 1) / 8)

typedef struct s_data_pattern {
    uint8_t pattern[PATTERN_MAX_SIZE];
    size_t  pattern_size; /* how many bytes pattern contains (capped at PATTERN_MAX_SIZE) */
} t_data_pattern;

typedef struct s_ft_ping_stat {
    size_t   packet_sent;
    size_t   packet_received;
    size_t   packet_duplicate;
    uint64_t last_packet_rtt;
    uint64_t min_packet_rtt;
    uint64_t max_packet_rtt;
} t_ft_ping_stat;

typedef struct s_ft_ping {
    uint64_t option_flags;
    struct {
        size_t   count;
        time_t   interval_between_packets;
        uint64_t packet_time_to_live;
        uint64_t packet_type_of_service;
        time_t   timeout;
        time_t   linger;

        t_data_pattern packet_data_pattern;
        size_t         packet_data_size;
        size_t         preload_nbr_packets;
    } options_value;     /* option value from argument parsing */
    t_ft_ping_stat stat; /* ping statistics */
    struct {
        uint16_t id;
        uint16_t nbr;
    } seq;
    uint8_t     seq_nbr_chk[SEQ_NBR_CHK_SIZE]; /* bit array to check for icmp echo reply sequence number duplicate */
    const char *node;                          /* node input from user*/
    char        p_inet_addr[INET_ADDRSTRLEN];  /* presentation internet address */
    struct sockaddr_in recvsock_addr;
    struct sockaddr_in hostsock_addr;
    socklen_t          sock_len;
    int                hostsock_fd;
} t_ft_ping;

typedef enum e_ping_state { RECV_MSG, SEND_MSG, END_PROGRAM } t_ping_state;

/**
 * @brief this global variable is used to track the state of the program, because the signal handler that catches
 * SIGARLM and SIGINT needs to inform the main ping routine on the action that shall be taken.
 * This mechanism is used to avoid any business logic in the signal handler, an avoid common mistake such as calling
 * malloc or free in a signal handler (which is not signal safe).
 *
 */
extern t_ping_state g_ping_state;

#endif  // FT_PING_H
