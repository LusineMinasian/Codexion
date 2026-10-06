/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lminasia <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/06 15:41:10 by lminasia          #+#    #+#             */
/*   Updated: 2026/10/06 15:43:48 by lminasia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	sim_mutexes_destroy(t_sim *sim)
{
	pthread_mutex_destroy(&sim->log_mutex);
	pthread_mutex_destroy(&sim->stop_mutex);
	pthread_mutex_destroy(&sim->ticket_mutex);
}

static int	sim_mutexes_init(t_sim *sim)
{
	if (pthread_mutex_init(&sim->log_mutex, NULL) != 0)
		return (0);
	if (pthread_mutex_init(&sim->stop_mutex, NULL) != 0)
	{
		pthread_mutex_destroy(&sim->log_mutex);
		return (0);
	}
	if (pthread_mutex_init(&sim->ticket_mutex, NULL) != 0)
	{
		pthread_mutex_destroy(&sim->log_mutex);
		pthread_mutex_destroy(&sim->stop_mutex);
		return (0);
	}
	return (1);
}

int	sim_init(t_sim *sim)
{
	gettimeofday(&sim->t0, NULL);
	sim->stop = 0;
	sim->next_ticket = 0;
	if (!sim_mutexes_init(sim))
		return (0);
	if (!dongles_init(sim))
	{
		sim_mutexes_destroy(sim);
		return (0);
	}
	if (!coders_init(sim))
	{
		dongles_destroy(sim);
		sim_mutexes_destroy(sim);
		return (0);
	}
	return (1);
}

void	sim_destroy(t_sim *sim)
{
	coders_destroy(sim);
	dongles_destroy(sim);
	sim_mutexes_destroy(sim);
}
