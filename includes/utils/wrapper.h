/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wrapper.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/30 09:02:39 by plouvel           #+#    #+#             */
/*   Updated: 2024/02/15 00:00:17 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @brief This file contains wrapper functions for system calls. These functions are wrapping logging and error.
 */

#ifndef UTILS_WRAPPER_H
#define UTILS_WRAPPER_H

#include <sys/socket.h>
#include <time.h>

#define RECVMSGW_INTERRUPT -2
#define RECVMSGW_TIMEOUT   -3

int     setsockopt_w(int sockfd, int level, int optname, const void *optval, socklen_t optlen);
ssize_t sendto_w(int fd, const void *buf, size_t n, int flags, const struct sockaddr *addr, socklen_t addr_len);
void   *malloc_w(size_t size);
int     timer_create_w(clockid_t clockid, struct sigevent *sevp, timer_t *timerid);
ssize_t recvmsg_w(int sockfd, struct msghdr *msg, int flags);

#endif /* !UTILS_WRAPPER_H */