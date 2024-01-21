/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   checksum.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/22 00:48:34 by plouvel           #+#    #+#             */
/*   Updated: 2024/01/22 00:49:43 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHECKSUM_H
#define CHECKSUM_H

#include <stddef.h>
#include <stdint.h>

uint16_t internet_checksum(void *payload, size_t payload_size);

#endif  // CHECKSUM_H