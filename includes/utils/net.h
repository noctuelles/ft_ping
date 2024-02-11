/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   net.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/11 10:53:28 by plouvel           #+#    #+#             */
/*   Updated: 2024/02/11 19:27:00 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef NET_H
#define NET_H

#include <arpa/inet.h>
#include <limits.h>
#include <stdbool.h>
#include <sys/socket.h>

#include "netinet/ip.h"

typedef struct s_sockaddr_str {
    char ip_str[INET6_ADDRSTRLEN];
    char hostname_str[HOST_NAME_MAX];
} t_sockaddr_str;

typedef enum e_sockaddr2str_ret {
    SOCKADDR2STR_OK,
    SOCKADDR2STR_ERR,
    SOCKADDR2STR_ONLY_NUMERIC,
} t_sockaddr2str_ret;

t_sockaddr2str_ret sockaddr2str(const struct sockaddr* sockaddr, socklen_t sock_len, t_sockaddr_str* sockaddr_str,
                                bool numeric_only);
void               print_ip_header_dump(struct ip* ip);

#endif /* NET_H */