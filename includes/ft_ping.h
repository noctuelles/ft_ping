/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ping.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/16 09:56:51 by plouvel           #+#    #+#             */
/*   Updated: 2024/02/14 23:50:11 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_PING_H
#define FT_PING_H

#include <limits.h>
#include <netinet/ip_icmp.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>

#include "translation.h"

#define CHK_SEQ_NBR(ft_ping, nbr) ((ft_ping)->icmp.seq.nbr_chk[nbr / 8] & (1U << nbr % 8))
#define SET_SEQ_NBR(ft_ping, nbr) ((ft_ping)->icmp.seq.nbr_chk[nbr / 8] ^= (1U << nbr % 8))

#define PATTERN_MAX_SIZE 16

#define DEFAULT_PACKET_DATA_SIZE    56
#define DEFAULT_PRELOAD_NBR_PACKETS 1
#define SEQ_NBR_CHK_SIZE            ((UINT16_MAX + 1) / sizeof(uint8_t))

typedef struct s_data_pattern {
    uint8_t pattern[PATTERN_MAX_SIZE];
    size_t  pattern_size; /* how many bytes pattern contains (capped at PATTERN_MAX_SIZE) */
} t_data_pattern;

typedef struct s_incoming_packet_info {
    struct ip*          ip;        /* Pointer to the first byte of the IP header*/
    struct icmp*        icmp;      /* Pointer to the first byte of the ICMP header */
    uint16_t            icmp_size; /* Size of the ICMP packet */
    struct sockaddr_in* from_sa;   /* Sender sockaddr */
} t_incoming_packet_info;

typedef struct s_ft_ping_stat {
    size_t packet_sent;
    size_t packet_received;
    size_t packet_duplicate;
    double last_packet_rtt;
    double min_packet_rtt;
    double max_packet_rtt;
    double avg_packet_rtt;
} t_ft_ping_stat;

typedef struct s_ft_ping_icmp {
    struct icmp* packet;
    size_t       packet_size;
    struct {
        uint16_t id;
        uint16_t nbr;
        uint8_t  nbr_chk[SEQ_NBR_CHK_SIZE]; /* bit array to check for icmp echo reply sequence number duplicate */
    } seq;
} t_ft_ping_icmp;

typedef struct s_ft_ping_options {
    bool count : 1;
    bool debug : 1;
    bool interval : 1;
    bool numeric : 1;
    bool time_to_live : 1;
    bool type_of_service : 1;
    bool verbose : 1;
    bool timeout : 1;
    bool linger : 1;
    bool flood : 1;
    bool preload : 1;
    bool pattern : 1;
    bool quiet : 1;
    bool ignore_routing : 1;
    bool size : 1;
    bool help : 1;
    bool node : 1;
} t_ft_ping_options;

typedef struct s_ft_ping_options_value {
    size_t         count;
    uint64_t       ms_interval_between_packets;
    time_t         timeout;
    time_t         linger;
    uint64_t       packet_time_to_live;
    uint64_t       packet_type_of_service;
    t_data_pattern packet_data_pattern;
    size_t         packet_data_size;
    size_t         preload_nbr_packets;
} t_ft_ping_options_value;

typedef struct s_ft_ping {
    t_ft_ping_options       options;
    t_ft_ping_options_value options_value;
    struct {
        struct sockaddr_in host;
        socklen_t          len;
        char               host_presentation[INET_ADDRSTRLEN];
    } sockaddr;
    t_ft_ping_icmp    icmp;
    t_ft_ping_stat    stat;
    const char*       node; /* node input from user */
    int               sock_fd;
    timer_t           timer_id;
    struct itimerspec timer_spec;
    struct timespec   start_time;
} t_ft_ping;

typedef enum e_ping_state { FINISHING, RUNNING_RECV, RUNNING_SEND, RUNNING_SEND_PRELOADING } t_ping_state;

/**
 * @brief this global variable is used to track if the ping routine should continue to run or not. This variable is
 * accessed only in the signal handler that catch SIGINT and SIGALRM, and in the ping routine.
 */
extern t_ping_state g_ping_state;

void ft_ping_clean(t_ft_ping* ft_ping);
int  ft_ping_init(t_ft_ping* ft_ping);
void ft_ping_print_intro(const t_ft_ping* ft_ping);
void ft_ping_print_outro(const t_ft_ping* ft_ping);

#endif  // FT_PING_H
