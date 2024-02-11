/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/17 16:15:44 by plouvel           #+#    #+#             */
/*   Updated: 2024/02/11 19:03:50 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <netinet/ip_icmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#include "ft_ping.h"
#include "libft.h"
#include "utils/wrapper.h"

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
    {ICMP_TIME_EXCEEDED, ICMP_EXC_TTL, "Time to Live Exceeded in Transit"},
    {ICMP_TIME_EXCEEDED, ICMP_EXC_FRAGTIME, "Fragment Reassembly Time Exceeded"},
};

#define NITEMS(x) (sizeof(x) / sizeof(x[0]))

/**
 * @brief compute the internet checksum of a payload for an ICMPv4 packet (ICMPv6 is computed by the kernel)
 * @note see https://tools.ietf.org/html/rfc1071
 *
 * @param payload pointer to the payload
 * @param payload_size size of the payload
 * @return uint16_t the checksum
 */
uint16_t
icmp_checksum(void *payload, size_t payload_size) {
    uint32_t  checksum = 0;
    uint16_t *ptr      = (uint16_t *)payload;

    payload_size /= sizeof(uint16_t);

    for (size_t i = 0; i < payload_size; i++) {
        checksum += ptr[i];
        checksum = (checksum & 0xffff) + (checksum >> 16);
    }

    return (uint16_t)(~checksum & 0xffff);
}

/**
 * @brief Decode an incoming IP datagram returned by the kernel. If successfull, the packet info pi is
 * updated.
 *
 * @param buffer pointer to buffer returned by the kernel.
 * @param buffer_size buffer size returned by the kernel.
 * @param pi pointer to the packet info.
 * @return int 0: the function sucessfully parsed the datagram. -1 : the packet is too short (fatal error), 1 : the
 * checksum doesn't match.
 */
int
icmp_packet_decode(uint8_t *buffer, size_t buffer_size, t_incoming_packet_info *pi) {
    struct ip   *ip               = NULL;
    struct icmp *icmp             = NULL;
    size_t       ip_header_len    = 0;
    size_t       icmp_payload_len = 0;
    uint16_t     saved_cksum      = 0;

    ip                   = (struct ip *)buffer;
    pi->ip               = ip;
    ip_header_len        = ip->ip_hl << 2U;
    icmp_payload_len     = ntohs(ip->ip_len) - ip_header_len;
    pi->icmp_payload_len = icmp_payload_len;
    if (buffer_size < ip_header_len + ICMP_MINLEN) {
        return (-1);
    }
    icmp             = (struct icmp *)(buffer + ip_header_len);
    pi->icmp         = icmp;
    saved_cksum      = icmp->icmp_cksum;
    icmp->icmp_cksum = 0;
    icmp->icmp_cksum = icmp_checksum(icmp, icmp_payload_len);
    if (icmp->icmp_cksum != saved_cksum) {
        return (1);
    }
    icmp->icmp_cksum = saved_cksum;
    return (0);
}

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

void
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