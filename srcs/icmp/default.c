/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   default.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/11 11:35:06 by plouvel           #+#    #+#             */
/*   Updated: 2024/02/11 20:36:26 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>

#include "ft_ping.h"
#include "icmp/utils.h"

void
print_icmp_default(const t_incoming_packet_info *pi, bool verbose, bool numeric_only) {
    t_sockaddr_str     sockaddr_str;
    t_sockaddr2str_ret ret;

    ret = sockaddr2str(&pi->from.sockaddr, pi->from.socklen, &sockaddr_str, numeric_only);
    if (ret == SOCKADDR2STR_ERR) {
        return;
    }
    if (ret == SOCKADDR2STR_ONLY_NUMERIC) {
        printf("%u bytes from %s: ", pi->icmp_payload_len, sockaddr_str.ip_str);
    } else if (ret == SOCKADDR2STR_OK) {
        printf("%u bytes from %s (%s): ", pi->icmp_payload_len, sockaddr_str.hostname_str, sockaddr_str.ip_str);
    }

    print_icmp_type_code_str(pi->icmp->icmp_type, pi->icmp->icmp_code);

    if (verbose) {
    }
}