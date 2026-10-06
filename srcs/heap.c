/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lminasia <lminasia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/06 15:43:48 by lminasia          #+#    #+#             */
/*   Updated: 2026/10/06 15:43:48 by lminasia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	heap_init(t_heap *h)
{
	h->capacity = 2;
	h->size = 0;
	h->nodes = malloc(sizeof(t_heap_node) * h->capacity);
	if (!h->nodes)
	{
		fprintf(stderr, "codexion: error\n");
		return (0);
	}
	return (1);
}

static int	heap_grow(t_heap *h)
{
	t_heap_node	*bigger;
	int			i;

	bigger = malloc(sizeof(t_heap_node) * h->capacity * 2);
	if (!bigger)
		return (0);
	i = 0;
	while (i < h->size)
	{
		bigger[i] = h->nodes[i];
		i++;
	}
	free(h->nodes);
	h->nodes = bigger;
	h->capacity = h->capacity * 2;
	return (1);
}

int	heap_push(t_heap *h, long key, long tiebreak, int coder_id)
{
	if (h->size == h->capacity && !heap_grow(h))
		return (0);
	h->nodes[h->size].key = key;
	h->nodes[h->size].tiebreak = tiebreak;
	h->nodes[h->size].coder_id = coder_id;
	h->size = h->size + 1;
	heap_sift_up(h, h->size - 1);
	return (1);
}

int	heap_pop_min(t_heap *h, t_heap_node *out)
{
	if (h->size == 0)
		return (0);
	*out = h->nodes[0];
	h->size = h->size - 1;
	h->nodes[0] = h->nodes[h->size];
	if (h->size > 0)
		heap_sift_down(h, 0);
	return (1);
}

void	heap_destroy(t_heap *h)
{
	free(h->nodes);
	h->nodes = NULL;
}
