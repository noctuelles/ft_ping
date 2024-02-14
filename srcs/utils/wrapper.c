/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wrapper.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/30 09:00:39 by plouvel           #+#    #+#             */
/*   Updated: 2024/02/15 00:00:17 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils/wrapper.h"

#include <errno.h>
#include <netdb.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <time.h>

#include "libft.h"

int
setsockopt_w(int sockfd, int level, int optname, const void *optval, socklen_t optlen) {
    if (setsockopt(sockfd, level, optname, optval, optlen) == -1) {
        ft_error(0, errno, "cannot set socket option");
        return (-1);
    }
    return (0);
}

void *
malloc_w(size_t size) {
    void *ptr = NULL;

    if ((ptr = malloc(size)) == NULL) {
        ft_error(0, errno, "cannot allocate memory");
    }

    return (ptr);
}

ssize_t
sendto_w(int fd, const void *buf, size_t n, int flags, const struct sockaddr *addr, socklen_t addr_len) {
    ssize_t ret = 0;

    if ((ret = sendto(fd, buf, n, flags, addr, addr_len)) == -1) {
        ft_error(0, errno, "cannot send packet");
    }

    return ret;
}

int
timer_create_w(clockid_t clockid, struct sigevent *sevp, timer_t *timerid) {
    int ret = 0;

    if ((ret = timer_create(clockid, sevp, timerid)) == -1) {
        ft_error(0, errno, "cannot create timer");
    }

    return ret;
}

ssize_t
recvmsg_w(int sockfd, struct msghdr *msg, int flags) {
    ssize_t ret = 0;

    if ((ret = recvmsg(sockfd, msg, flags)) == -1) {
        if (errno != EAGAIN && errno != EWOULDBLOCK && errno != EINTR) {
            ft_error(0, errno, "cannot receive packet");
        } else if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return RECVMSGW_TIMEOUT;
        } else {
            return RECVMSGW_INTERRUPT;
        }
    }

    return ret;
}