/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   translation.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/19 16:00:16 by plouvel           #+#    #+#             */
/*   Updated: 2024/02/13 11:31:06 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "translation.h"

#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "libft.h"

static struct addrinfo *
ipv4_icmp_getaddrinfo(const char *node) {
    struct addrinfo  hints = {0};
    struct addrinfo *res   = NULL;
    int              error = 0;

    hints.ai_family   = AF_INET;
    hints.ai_socktype = SOCK_RAW;
    hints.ai_protocol = IPPROTO_ICMP;
    error             = getaddrinfo(node, NULL, &hints, &res);
    if (error != 0) {
        ft_error(0, 0, "cannot resolve node: %s", gai_strerror(error));
        return (NULL);
    }
    return (res);
}

static int
get_socket_from_addrinfo_list(struct addrinfo *res, struct sockaddr_in *sockaddr) {
    int fd = -1;

    for (struct addrinfo *rp = res; rp != NULL; rp = rp->ai_next) {
        fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (fd != -1) {
            *sockaddr = *((struct sockaddr_in *)rp->ai_addr);
            break;
        }
    }
    if (fd == -1) {
        ft_error(0, errno, "cannot create socket");
    }
    return (fd);
}

int
get_socket_from_node(const char *node, struct sockaddr_in *sockaddr) {
    struct addrinfo *res = NULL;
    int              fd  = -1;

    res = ipv4_icmp_getaddrinfo(node);
    if (res == NULL) {
        return (-1);
    }
    fd = get_socket_from_addrinfo_list(res, sockaddr);

    freeaddrinfo(res);
    return (fd);
}