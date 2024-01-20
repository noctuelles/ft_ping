/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_args_parser_fn.h                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/16 09:56:40 by plouvel           #+#    #+#             */
/*   Updated: 2024/01/20 14:00:42 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_ARGS_PARSER_FN_H
#define FT_ARGS_PARSER_FN_H

#include "ft_args_parser.h"

int parse_count(char *argument, t_args_parser_state *parser_state, void *input);
int parse_preload(char *argument, t_args_parser_state *parser_state, void *input);
int parse_timeout(char *argument, t_args_parser_state *parser_state, void *input);
int parse_help(char *argument, t_args_parser_state *parser_state, void *input);
int parse_arguments(char *argument, t_args_parser_state *parser_state, void *input);
int parse_packet_size(char *argument, t_args_parser_state *parser_state, void *input);
int parse_data_pattern(char *argument, t_args_parser_state *parser_state, void *input);

#endif  // FT_ARGS_PARSER_FN_H