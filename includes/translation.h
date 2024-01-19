/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   translation.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/19 16:47:48 by plouvel           #+#    #+#             */
/*   Updated: 2024/01/19 16:53:58 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TRANSLATION_H
#define TRANSLATION_H

#include <netdb.h>

typedef struct s_socket {
    int             fd;
    char            presentation[INET_ADDRSTRLEN];
    struct addrinfo addrinfo;
} t_socket;

int get_socket_from_node(const char *node, t_socket *socket);

#endif  // TRANSLATION_H