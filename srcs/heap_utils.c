/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lminasia <lminasia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/06 15:43:48 by lminasia          #+#    #+#             */
/*   Updated: 2026/10/06 15:43:48 by lminasia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	node_less(t_heap_node *a, t_heap_node *b)
{
	if (a->key != b->key)
		return (a->key < b->key);
	return (a->tiebreak < b->tiebreak);
}

void	heap_sift_up(t_heap *h, int i)
{
	int			parent;
	t_heap_node	tmp;

	while (i > 0)
	{
		parent = (i - 1) / 2;
		if (!node_less(&h->nodes[i], &h->nodes[parent]))
			return ;
		tmp = h->nodes[i];
		h->nodes[i] = h->nodes[parent];
		h->nodes[parent] = tmp;
		i = parent;
	}
}

void	heap_sift_down(t_heap *h, int i)
{
	int			smallest;
	t_heap_node	tmp;

	while (1)
	{
		smallest = i;
		if (2 * i + 1 < h->size
			&& node_less(&h->nodes[2 * i + 1], &h->nodes[smallest]))
			smallest = 2 * i + 1;
		if (2 * i + 2 < h->size
			&& node_less(&h->nodes[2 * i + 2], &h->nodes[smallest]))
			smallest = 2 * i + 2;
		if (smallest == i)
			return ;
		tmp = h->nodes[i];
		h->nodes[i] = h->nodes[smallest];
		h->nodes[smallest] = tmp;
		i = smallest;
	}
}

int	heap_peek_min(t_heap *h, t_heap_node *out)
{
	if (h->size == 0)
		return (0);
	*out = h->nodes[0];
	return (1);
}

int	heap_remove_coder(t_heap *h, int coder_id)
{
	int	i;

	i = 0;
	while (i < h->size && h->nodes[i].coder_id != coder_id)
		i++;
	if (i == h->size)
		return (0);
	h->size = h->size - 1;
	h->nodes[i] = h->nodes[h->size];
	heap_sift_up(h, i);
	heap_sift_down(h, i);
	return (1);
}
