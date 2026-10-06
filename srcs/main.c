/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lminasia <lminasia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/06 15:43:48 by lminasia          #+#    #+#             */
/*   Updated: 2026/10/06 15:43:48 by lminasia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	abort_start(t_sim *sim, int started)
{
	int	i;

	fprintf(stderr, "codexion: pthread_create failed\n");
	sim_request_stop(sim);
	wake_all_dongles(sim);
	i = 0;
	while (i < started)
	{
		pthread_join(sim->coders[i].thread, NULL);
		i++;
	}
}

static int	start_threads(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->number_of_coders)
	{
		if (pthread_create(&sim->coders[i].thread, NULL,
				coder_routine, &sim->coders[i]) != 0)
		{
			abort_start(sim, i);
			return (0);
		}
		i++;
	}
	if (pthread_create(&sim->monitor, NULL, monitor_routine, sim) != 0)
	{
		abort_start(sim, i);
		return (0);
	}
	return (1);
}

static void	join_threads(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->number_of_coders)
	{
		pthread_join(sim->coders[i].thread, NULL);
		i++;
	}
	pthread_join(sim->monitor, NULL);
}

int	main(int argc, char **argv)
{
	t_sim	sim;

	if (!parse_args(argc, argv, &sim))
		return (1);
	if (!sim_init(&sim))
		return (1);
	if (!start_threads(&sim))
	{
		sim_destroy(&sim);
		return (1);
	}
	join_threads(&sim);
	sim_destroy(&sim);
	return (0);
}
