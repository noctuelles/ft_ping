/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   net.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/10 18:10:34 by plouvel           #+#    #+#             */
/*   Updated: 2024/02/11 19:34:34 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils/net.h"

#include <arpa/inet.h>
#include <errno.h>
#include <limits.h>
#include <netdb.h>
#include <stdio.h>
#include <sys/socket.h>

#include "libft.h"
#include "netinet/ip.h"
#include "netinet/ip_icmp.h"
#include "utils/wrapper.h"

/**
 * @brief Translate a sockaddr struct to a human readable string.
 * @note The called should call free on the returned pointer.
 *
 * @param sockaddr pointer to a sockaddr struct
 * @param sock_len size of the sockaddr struct
 * @param numeric_only if true, only the numeric address is returned, else, the hostname and the numeric address are
 * returned in this format : "ipaddress (hostname)"
 * @return char*
 */
t_sockaddr2str_ret
sockaddr2str(const struct sockaddr* sockaddr, socklen_t sock_len, t_sockaddr_str* sockaddr_str, bool numeric_only) {
    if (getnameinfo_w(sockaddr, sock_len, sockaddr_str->ip_str, sizeof(sockaddr_str->ip_str), NULL, 0,
                      NI_NUMERICHOST) != 0) {
        return (SOCKADDR2STR_ERR);
    }
    if (numeric_only) {
        return (SOCKADDR2STR_ONLY_NUMERIC);
    }
    if (getnameinfo(sockaddr, sock_len, sockaddr_str->hostname_str, sizeof(sockaddr_str->hostname_str), NULL, 0,
                    NI_NAMEREQD) != 0) {
        return (SOCKADDR2STR_ONLY_NUMERIC); /* Fail without printing any error message. */
    }
    return (SOCKADDR2STR_OK);
}
