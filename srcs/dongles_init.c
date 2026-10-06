/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongles_init.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lminasia <lminasia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/06 15:43:48 by lminasia          #+#    #+#             */
/*   Updated: 2026/10/06 15:43:48 by lminasia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	dongle_init_one(t_dongle *d, int id, t_sim *sim)
{
	d->id = id;
	d->taken = 0;
	d->available_at = 0;
	d->sim = sim;
	if (!heap_init(&d->waiters))
		return (0);
	if (pthread_mutex_init(&d->mutex, NULL) != 0)
	{
		heap_destroy(&d->waiters);
		return (0);
	}
	if (pthread_cond_init(&d->cond, NULL) != 0)
	{
		pthread_mutex_destroy(&d->mutex);
		heap_destroy(&d->waiters);
		return (0);
	}
	return (1);
}

static void	dongles_destroy_n(t_sim *sim, int n)
{
	int	i;

	i = 0;
	while (i < n)
	{
		pthread_mutex_destroy(&sim->dongles[i].mutex);
		pthread_cond_destroy(&sim->dongles[i].cond);
		heap_destroy(&sim->dongles[i].waiters);
		i++;
	}
	free(sim->dongles);
	sim->dongles = NULL;
}

int	dongles_init(t_sim *sim)
{
	int	i;

	sim->dongles = malloc(sizeof(t_dongle) * sim->number_of_coders);
	if (!sim->dongles)
	{
		fprintf(stderr, "codexion: error\n");
		return (0);
	}
	i = 0;
	while (i < sim->number_of_coders)
	{
		if (!dongle_init_one(&sim->dongles[i], i, sim))
		{
			dongles_destroy_n(sim, i);
			fprintf(stderr, "codexion: dongle init failed\n");
			return (0);
		}
		i++;
	}
	return (1);
}

void	dongles_destroy(t_sim *sim)
{
	dongles_destroy_n(sim, sim->number_of_coders);
}

void	dongle_release(t_dongle *d)
{
	pthread_mutex_lock(&d->mutex);
	d->taken = 0;
	d->available_at = time_now_ms(d->sim) + d->sim->dongle_cooldown;
	pthread_cond_broadcast(&d->cond);
	pthread_mutex_unlock(&d->mutex);
}
