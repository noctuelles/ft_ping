/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/30 05:12:35 by plouvel           #+#    #+#             */
/*   Updated: 2024/01/30 05:26:27 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern char *program_invocation_short_name;

void
ft_error(int status, int errnum, const char *format, ...) {
    va_list ap;

    va_start(ap, format);
    fflush(stdout);
    (void)fprintf(stderr, "%s: ", program_invocation_short_name);
    (void)vfprintf(stderr, format, ap);
    if (errnum != 0)
        (void)fprintf(stderr, ": %s", strerror(errnum));
    (void)fprintf(stderr, "\n");
    va_end(ap);
    if (status != 0) {
        exit(status);
    }
}