/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   logger.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lminasia <lminasia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/06 15:43:48 by lminasia          #+#    #+#             */
/*   Updated: 2026/10/06 15:43:48 by lminasia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	log_event(t_sim *sim, int coder_id, const char *msg)
{
	pthread_mutex_lock(&sim->log_mutex);
	if (!sim_should_stop(sim))
		printf("%ld %d %s\n", time_now_ms(sim), coder_id, msg);
	pthread_mutex_unlock(&sim->log_mutex);
}

/* Stop and print under log_mutex so nothing can be logged after it. */
void	log_burnout(t_sim *sim, int coder_id)
{
	pthread_mutex_lock(&sim->log_mutex);
	if (!sim_should_stop(sim))
	{
		sim_request_stop(sim);
		printf("%ld %d burned out\n", time_now_ms(sim), coder_id);
	}
	pthread_mutex_unlock(&sim->log_mutex);
}
