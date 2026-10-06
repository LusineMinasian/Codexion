/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lminasia <lminasia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/06 15:43:48 by lminasia          #+#    #+#             */
/*   Updated: 2026/10/06 15:43:48 by lminasia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <limits.h>

static int	parse_uint(const char *s, long *out)
{
	long	val;

	if (s == NULL || *s == '\0')
		return (0);
	val = 0;
	while (*s)
	{
		if (*s < '0' || *s > '9')
			return (0);
		val = val * 10 + (*s - '0');
		if (val > INT_MAX)
			return (0);
		s++;
	}
	*out = val;
	return (1);
}

static int	parse_field(const char *s, long *out, long min, const char *name)
{
	if (!parse_uint(s, out) || *out < min)
	{
		fprintf(stderr, "codexion: invalid %s\n", name);
		return (0);
	}
	return (1);
}

static int	parse_numbers(char **argv, t_sim *sim)
{
	long	v[7];

	if (!parse_field(argv[1], &v[0], 1, "number_of_coders")
		|| !parse_field(argv[2], &v[1], 1, "time_to_burnout")
		|| !parse_field(argv[3], &v[2], 1, "time_to_compile")
		|| !parse_field(argv[4], &v[3], 0, "time_to_debug")
		|| !parse_field(argv[5], &v[4], 0, "time_to_refactor")
		|| !parse_field(argv[6], &v[5], 0, "number_of_compiles_required")
		|| !parse_field(argv[7], &v[6], 0, "dongle_cooldown"))
		return (0);
	sim->number_of_coders = (int)v[0];
	sim->time_to_burnout = v[1];
	sim->time_to_compile = v[2];
	sim->time_to_debug = v[3];
	sim->time_to_refactor = v[4];
	sim->number_of_compiles_required = (int)v[5];
	sim->dongle_cooldown = v[6];
	return (1);
}

int	parse_args(int argc, char **argv, t_sim *sim)
{
	if (argc != 9)
	{
		fprintf(stderr, "codexion: expected 8 arguments\n");
		return (0);
	}
	if (!parse_numbers(argv, sim))
		return (0);
	if (strcmp(argv[8], "fifo") == 0)
		sim->scheduler = SCHEDULER_FIFO;
	else if (strcmp(argv[8], "edf") == 0)
		sim->scheduler = SCHEDULER_EDF;
	else
	{
		fprintf(stderr, "codexion: scheduler must be 'fifo' or 'edf'\n");
		return (0);
	}
	return (1);
}
