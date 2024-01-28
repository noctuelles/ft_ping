/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   checksum.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/22 00:48:08 by plouvel           #+#    #+#             */
/*   Updated: 2024/01/27 10:06:15 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stddef.h>
#include <stdint.h>

/**
 * @brief compute the internet checksum of a payload for an ICMPv4 packet (ICMPv6 is computed by the kernel)
 * @note see https://tools.ietf.org/html/rfc1071
 *
 * @param payload pointer to the payload
 * @param payload_size size of the payload
 * @return uint16_t the checksum
 */
uint16_t
icmp_checksum(void *payload, size_t payload_size) {
    uint32_t  checksum = 0;
    uint16_t *ptr      = (uint16_t *)payload;

    payload_size /= sizeof(uint16_t);

    for (size_t i = 0; i < payload_size; i++) {
        checksum += ptr[i];
        checksum = (checksum & 0xffff) + (checksum >> 16);
    }

    return (uint16_t)(~checksum & 0xffff);
}