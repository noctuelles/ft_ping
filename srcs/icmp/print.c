/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   output.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/14 22:55:26 by plouvel           #+#    #+#             */
/*   Updated: 2024/02/14 23:39:55 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ip/print.h"

#include <arpa/inet.h>
#include <limits.h>
#include <netinet/ip_icmp.h>
#include <stdio.h>
#include <stdlib.h>

#include "ft_ping.h"

struct s_icmp_diagnostic {
    uint8_t type;     /* Type code */
    char   *type_str; /* Type string*/
    char   *prefix;   /* When type_str is NULL, it means that the type doesn't have any meaning without a code and must
                         refer to the structure below. */
};

struct s_icmp_diagnostic_code {
    uint8_t type;
    uint8_t code;
    char   *code_str;
};

/* https://en.wikipedia.org/wiki/Internet_Control_Message_Protocol#header_type */

static struct s_icmp_diagnostic g_icmp_diagnostics[] = {
    {ICMP_ECHOREPLY, "Echo Reply", NULL},
    {ICMP_DEST_UNREACH, NULL, "Destination Unreachable"},
    {ICMP_SOURCE_QUENCH, "Source Quench", NULL},
    {ICMP_REDIRECT, NULL, "Redirect"},
    {ICMP_ECHO, "Echo Request", NULL},
    {ICMP_ROUTERADVERT, "Router Advertisement", NULL},
    {ICMP_ROUTERSOLICIT, "Router Solicitation", NULL},
    {ICMP_TIME_EXCEEDED, NULL, "Time Exceeded"},
    {ICMP_PARAMETERPROB, "Parameter Problem", NULL},
    {ICMP_TIMESTAMP, "Timestamp Request", NULL},
};

static struct s_icmp_diagnostic_code g_icmp_diagnostic_code[] = {
    {ICMP_DEST_UNREACH, ICMP_NET_UNREACH, "Destination Network Unreachable"},
    {ICMP_DEST_UNREACH, ICMP_HOST_UNREACH, "Destination Host Unreachable"},
    {ICMP_DEST_UNREACH, ICMP_PROT_UNREACH, "Destination Protocol Unreachable"},
    {ICMP_DEST_UNREACH, ICMP_PORT_UNREACH, "Destination Port Unreachable"},
    {ICMP_DEST_UNREACH, ICMP_FRAG_NEEDED, "Fragmentation Required, and DF flag set"},
    {ICMP_DEST_UNREACH, ICMP_SR_FAILED, "Source Route Failed"},
    {ICMP_DEST_UNREACH, ICMP_NET_UNKNOWN, "Destination Network Unknown"},
    {ICMP_DEST_UNREACH, ICMP_HOST_UNKNOWN, "Destination Host Unknown"},
    {ICMP_DEST_UNREACH, ICMP_HOST_ISOLATED, "Source Host Isolated"},
    {ICMP_DEST_UNREACH, ICMP_NET_ANO, "Network Administratively Prohibited"},
    {ICMP_DEST_UNREACH, ICMP_HOST_ANO, "Host Administratively Prohibited"},
    {ICMP_DEST_UNREACH, ICMP_NET_UNR_TOS, "Network Unreachable for Type of Service"},
    {ICMP_DEST_UNREACH, ICMP_HOST_UNR_TOS, "Host Unreachable for Type of Service"},
    {ICMP_DEST_UNREACH, ICMP_PKT_FILTERED, "Packet Filtered"},
    {ICMP_DEST_UNREACH, ICMP_PREC_VIOLATION, "Precedence Violation"},
    {ICMP_DEST_UNREACH, ICMP_PREC_CUTOFF, "Precedence Cutoff"},
    {ICMP_REDIRECT, ICMP_REDIR_NET, "Redirect Datagram for the Network"},
    {ICMP_REDIRECT, ICMP_REDIR_HOST, "Redirect Datagram for the Host"},
    {ICMP_REDIRECT, ICMP_REDIR_NETTOS, "Redirect Datagram for the Type of Service and Network"},
    {ICMP_REDIRECT, ICMP_REDIR_HOSTTOS, "Redirect Datagram for the Type of Service and Host"},
    {ICMP_TIME_EXCEEDED, ICMP_EXC_TTL, "Time to Live Exceeded"},
    {ICMP_TIME_EXCEEDED, ICMP_EXC_FRAGTIME, "Fragment Reassembly Time Exceeded"},
};

#define NITEMS(x) (sizeof(x) / sizeof(x[0]))

static void
print_icmp_code(uint8_t type, uint8_t code, char *prefix) {
    for (size_t i = 0; i < NITEMS(g_icmp_diagnostic_code); i++) {
        if (g_icmp_diagnostic_code[i].type == type && g_icmp_diagnostic_code[i].code == code) {
            printf("%s\n", g_icmp_diagnostic_code[i].code_str);
            return;
        }
    }

    printf("%s, Bad icmp code : %u\n", prefix, code);
}

static void
print_icmp_type_code_str(uint8_t type, uint8_t code) {
    for (size_t i = 0; i < NITEMS(g_icmp_diagnostics); i++) {
        if (g_icmp_diagnostics[i].type == type) {
            if (g_icmp_diagnostics[i].type_str != NULL) {
                printf("%s\n", g_icmp_diagnostics[i].type_str);
            } else {
                print_icmp_code(type, code, g_icmp_diagnostics[i].prefix);
            }
            return;
        }
    }
    printf("Bad icmp type : %u\n", type);
}

static void
print_icmp_header_details(const struct icmp *icmp) {
    const struct ip   *orig_ip_packet   = NULL;
    const struct icmp *orig_icmp_packet = NULL;

    orig_ip_packet   = (const struct ip *)icmp->icmp_dun.id_data;
    orig_icmp_packet = (const struct icmp *)(icmp->icmp_dun.id_data + (orig_ip_packet->ip_hl << 2));

    printf("ICMP: type %u, code %u, size %u, id 0x%04x, seq 0x%04x\n", orig_icmp_packet->icmp_type,
           orig_icmp_packet->icmp_code, ntohs(orig_ip_packet->ip_len) - (orig_ip_packet->ip_hl << 2),
           ntohs(orig_icmp_packet->icmp_hun.ih_idseq.icd_id), ntohs(orig_icmp_packet->icmp_hun.ih_idseq.icd_seq));
}

void
print_icmp_default(const t_incoming_packet_info *pi, bool verbose, bool numeric_only) {
    char ip_str[INET_ADDRSTRLEN]                     = {0};
    char hostname_str[HOST_NAME_MAX]                 = {0};
    char buffer[INET_ADDRSTRLEN + HOST_NAME_MAX + 4] = {0}; /* + 4 : a pair of parenthesis space, NUL terminator */
    bool hostname_resolved                           = false;

    (void)inet_ntop(AF_INET, &pi->from_sa->sin_addr, ip_str, INET_ADDRSTRLEN);
    if (!numeric_only) {
        hostname_resolved = getnameinfo((const struct sockaddr *)pi->from_sa, sizeof(*pi->from_sa), hostname_str,
                                        HOST_NAME_MAX, NULL, 0, NI_NAMEREQD) == 0;
    }
    if (hostname_resolved) {
        snprintf(buffer, sizeof(buffer), "%s (%s)", hostname_str, ip_str);
    } else {
        snprintf(buffer, sizeof(buffer), "%s", ip_str);
    }
    printf("%u bytes from %s: ", pi->icmp_size, buffer);
    print_icmp_type_code_str(pi->icmp->icmp_type, pi->icmp->icmp_code);
    if (verbose) {
        print_ip_header_hexdump(pi->ip);
        print_ip_header(pi->ip);
        print_icmp_header_details(pi->icmp);
    }
}

void
print_icmp_echo_reply(const t_ft_ping *ft_ping, const t_incoming_packet_info *pi, bool duplicate) {
    if (ft_ping->options.quiet) {
        return;
    }
    if (ft_ping->options.flood) {
        printf("\b");
        return;
    }
    printf("%u bytes from %s: icmp_seq=%u ttl=%u", pi->icmp_size, inet_ntoa(pi->from_sa->sin_addr),
           ntohs(pi->icmp->icmp_hun.ih_idseq.icd_seq), pi->ip->ip_ttl);
    if (pi->icmp_size - ICMP_MINLEN >= (uint16_t)sizeof(struct timespec)) {
        printf(" time=%.3f ms", ft_ping->stat.last_packet_rtt);
    }
    if (duplicate) {
        printf(" (DUP!)");
    }
    printf("\n");
}
