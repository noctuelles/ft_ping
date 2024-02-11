/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/11 19:34:25 by plouvel           #+#    #+#             */
/*   Updated: 2024/02/11 19:35:16 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <netinet/ip.h>
#include <stdio.h>

void
print_ip_header_hexdump(struct ip* ip) {
    printf("IP Hdr Dump:\n");
    for (size_t i = 0; i < sizeof(*ip); i++) {
        printf("%02x%s", *((uint8_t*)ip + i), i % 2 != 0 ? " " : "");
    }
    printf("\n");
}

void
print_ip_header(struct ip* ip)