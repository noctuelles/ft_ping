/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/14 23:00:25 by plouvel           #+#    #+#             */
/*   Updated: 2024/02/14 23:09:30 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_PING_PRINT_H
#define FT_PING_PRINT_H

#include <netinet/ip.h>

void print_ip_header_hexdump(const struct ip* ip);
void print_ip_header(const struct ip* ip);

#endif  // FT_PING_PRINT_H
