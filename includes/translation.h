/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   translation.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/19 16:47:48 by plouvel           #+#    #+#             */
/*   Updated: 2024/01/27 14:07:22 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TRANSLATION_H
#define TRANSLATION_H

#include <netdb.h>

int get_socket_from_node(const char *node, struct sockaddr_in *sockaddr);

#endif  // TRANSLATION_H