/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   translation.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/19 16:00:16 by plouvel           #+#    #+#             */
/*   Updated: 2024/01/19 17:09:17 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "translation.h"

#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

static struct addrinfo *
ipv4_icmp_getaddrinfo(const char *node) {
    struct addrinfo  hints = {0};
    struct addrinfo *res   = NULL;
    int              error = 0;

    hints.ai_family   = AF_INET;
    hints.ai_socktype = SOCK_RAW;
    hints.ai_protocol = IPPROTO_ICMP;

    error = getaddrinfo(node, NULL, &hints, &res);
    if (error != 0) {
        fprintf(stderr, "ft_ping: %s\n", gai_strerror(error));
        return (NULL);
    }

    return (res);
}

static int
get_socket_from_addrinfo_list(struct addrinfo *res, t_socket *psocket) {
    int fd = -1;

    for (struct addrinfo *rp = res; rp != NULL; rp = rp->ai_next) {
        fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (fd != -1) {
            psocket->fd       = fd;
            psocket->addrinfo = *rp;
            break;
        }
    }

    if (fd == -1) {
        fprintf(stderr, "ft_ping: %s\n", strerror(errno));
    }

    return (fd);
}

int
get_socket_from_node(const char *node, t_socket *socket) {
    struct addrinfo *res = NULL;
    int              fd  = -1;

    res = ipv4_icmp_getaddrinfo(node);
    if (res == NULL) {
        return (-1);
    }

    fd = get_socket_from_addrinfo_list(res, socket);
    (void)inet_ntop(AF_INET, &((struct sockaddr_in *)&socket->addrinfo.ai_addr)->sin_addr, socket->presentation,
                    INET_ADDRSTRLEN);

    freeaddrinfo(res);

    return (fd);
}