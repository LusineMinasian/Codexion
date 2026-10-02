#include "codexion.h"

int heap_init(t_heap *h)
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

static int heap_grow(t_heap *h)
{
    t_heap_node *bigger;
    int         i;

    h->capacity = h->capacity * 2;
    bigger = malloc(sizeof(t_heap_node) * h->capacity);
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
    return (1);
}

static int node_less(t_heap_node *a, t_heap_node *b)
{
    if (a->key != b->key)
    {
        if (a->key < b->key)
            return (1);
    }
    else
    {
        if (a->tiebreak < b->tiebreak)
            return (1);
    }
    return (0);
}

static void swap_nodes(t_heap_node *a, t_heap_node *b)
{
    t_heap_node temp;

    temp = *a;
    *a = *b;
    *b = temp;
}

static void sift_up(t_heap *h, int i)
{
    int parent;

    while (i > 0)
    {
        parent = (i - 1) / 2;
        if (node_less(&h->nodes[i], &h->nodes[parent]))
        {
            swap_nodes(&h->nodes[i], &h->nodes[parent]);
            i = parent;
        }
        else
            return ;
    }
}

int heap_push(t_heap *h, long key, long tiebreak, int coder_id)
{
    if (h->size == h->capacity)
    {
        if (heap_grow(h) == 0)
            return (0);
    }
    h->nodes[h->size].key = key;
    h->nodes[h->size].tiebreak = tiebreak;
    h->nodes[h->size].coder_id = coder_id;
    h->size = h->size + 1;
    sift_up(h, h->size - 1);
    return (1);
}