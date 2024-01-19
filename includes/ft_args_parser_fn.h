/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_args_parser_fn.h                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/16 09:56:40 by plouvel           #+#    #+#             */
/*   Updated: 2024/01/19 15:33:43 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_ARGS_PARSER_FN_H
#define FT_ARGS_PARSER_FN_H

#include "ft_args_parser.h"

int parse_count(const char *argument, t_args_parser_state *parser_state, void *input);
int parse_preload(const char *argument, t_args_parser_state *parser_state, void *input);
int parse_timeout(const char *argument, t_args_parser_state *parser_state, void *input);
int parse_help(const char *argument, t_args_parser_state *parser_state, void *input);
int parse_arguments(const char *argument, t_args_parser_state *parser_state, void *input);
int parse_packet_size(const char *argument, t_args_parser_state *parser_state, void *input);

#endif  // FT_ARGS_PARSER_FN_H