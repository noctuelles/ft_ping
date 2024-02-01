/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/01 06:13:30 by plouvel           #+#    #+#             */
/*   Updated: 2024/02/01 06:51:18 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ICMP_ECHO_H
#define ICMP_ECHO_H

#include "ft_ping.h"

int icmp_echo_send_packet(t_ft_ping *ft_ping);

#endif /* ICMP_ECHO_H */