/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_args_parser_fn.h                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/16 09:56:40 by plouvel           #+#    #+#             */
/*   Updated: 2024/01/28 11:03:17 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_ARGS_PARSER_FN_H
#define FT_ARGS_PARSER_FN_H

#include "ft_args_parser.h"

/* Option valid for all request types */

int parse_count(char *argument, t_args_parser_state *parser_state, void *input);
int parse_debug(char *argument, t_args_parser_state *parser_state, void *input);
int parse_interval(char *argument, t_args_parser_state *parser_state, void *input);
int parse_numeric(char *argument, t_args_parser_state *parser_state, void *input);
int parse_ignore_routing(char *argument, t_args_parser_state *parser_state, void *input);
int parse_time_to_live(char *argument, t_args_parser_state *parser_state, void *input);
int parse_type_of_service(char *argument, t_args_parser_state *parser_state, void *input);
int parse_verbose(char *argument, t_args_parser_state *parser_state, void *input);
int parse_timeout(char *argument, t_args_parser_state *parser_state, void *input);
int parse_linger(char *argument, t_args_parser_state *parser_state, void *input);

/* END */

/* Option valid for ECHO_REQUEST */

int parse_flood(char *argument, t_args_parser_state *parser_state, void *input);
int parse_ip_timestamp(char *argument, t_args_parser_state *parser_state, void *input);
int parse_preload(char *argument, t_args_parser_state *parser_state, void *input);
int parse_pattern(char *argument, t_args_parser_state *parser_state, void *input);
int parse_quiet(char *argument, t_args_parser_state *parser_state, void *input);
int parse_route(char *argument, t_args_parser_state *parser_state, void *input);
int parse_size(char *argument, t_args_parser_state *parser_state, void *input);

/* END */

int parse_arguments(char *argument, t_args_parser_state *parser_state, void *input);
int parse_help(char *argument, t_args_parser_state *parser_state, void *input);

#endif  // FT_ARGS_PARSER_FN_H