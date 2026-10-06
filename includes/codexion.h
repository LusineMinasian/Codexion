/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lminasia <lminasia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/06 15:43:48 by lminasia          #+#    #+#             */
/*   Updated: 2026/10/06 15:43:48 by lminasia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <stddef.h>
# include <stdlib.h>
# include <stdio.h>
# include <string.h>
# include <pthread.h>
# include <sys/time.h>
# include <unistd.h>

typedef struct s_sim	t_sim;

typedef enum e_scheduler
{
	SCHEDULER_FIFO,
	SCHEDULER_EDF
}	t_scheduler;

typedef struct s_heap_node
{
	long	key;
	long	tiebreak;
	int		coder_id;
}	t_heap_node;

typedef struct s_heap
{
	t_heap_node	*nodes;
	int			size;
	int			capacity;
}	t_heap;

typedef struct s_dongle
{
	int				id;
	int				taken;
	long			available_at;
	pthread_mutex_t	mutex;
	pthread_cond_t	cond;
	t_heap			waiters;
	t_sim			*sim;
}	t_dongle;

typedef struct s_coder
{
	int				id;
	int				first;
	int				second;
	int				compile_count;
	long			last_compile_start;
	pthread_mutex_t	mutex;
	pthread_t		thread;
	t_sim			*sim;
}	t_coder;

struct s_sim
{
	int				number_of_coders;
	long			time_to_burnout;
	long			time_to_compile;
	long			time_to_debug;
	long			time_to_refactor;
	int				number_of_compiles_required;
	long			dongle_cooldown;
	t_scheduler		scheduler;
	t_dongle		*dongles;
	struct timeval	t0;
	pthread_mutex_t	log_mutex;
	t_coder			*coders;
	int				stop;
	pthread_mutex_t	stop_mutex;
	long			next_ticket;
	pthread_mutex_t	ticket_mutex;
	pthread_t		monitor;
};

int		parse_args(int argc, char **argv, t_sim *sim);
int		heap_init(t_heap *h);
int		heap_push(t_heap *h, long key, long tiebreak, int coder_id);
int		heap_pop_min(t_heap *h, t_heap_node *out);
int		heap_peek_min(t_heap *h, t_heap_node *out);
int		heap_remove_coder(t_heap *h, int coder_id);
void	heap_sift_up(t_heap *h, int i);
void	heap_sift_down(t_heap *h, int i);
void	heap_destroy(t_heap *h);
void	dongle_release(t_dongle *d);
int		dongles_init(t_sim *sim);
void	dongles_destroy(t_sim *sim);
long	time_now_ms(t_sim *sim);
void	log_event(t_sim *sim, int coder_id, const char *msg);
void	log_burnout(t_sim *sim, int coder_id);
void	sim_sleep(t_sim *sim, long ms);
void	ms_to_timespec(t_sim *sim, long ms, struct timespec *ts);
int		dongles_take(t_dongle *a, t_dongle *b, t_coder *c);
int		sim_should_stop(t_sim *sim);
void	sim_request_stop(t_sim *sim);
void	*coder_routine(void *arg);
void	coders_destroy(t_sim *sim);
int		coders_init(t_sim *sim);
void	*monitor_routine(void *arg);
int		sim_init(t_sim *sim);
void	sim_destroy(t_sim *sim);
void	wake_all_dongles(t_sim *sim);

#endif
