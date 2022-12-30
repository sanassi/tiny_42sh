#include "hash_map.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct hash_map *hash_map_init(size_t size)
{
    struct hash_map *map = malloc(sizeof(struct hash_map));
    if (map == NULL)
    {
        return NULL;
    }
    map->size = size;

    map->data = calloc(size, sizeof(struct pair_list));

    if (map->data == NULL)
    {
        return NULL;
    }

    return map;
}

bool hm_insert_int(struct hash_map *h, const char *key, int value, bool *updated)
{
    char *to_str = my_itoa(value);
    bool res = hash_map_insert(h, key, to_str, updated);
    return res;
}

bool hash_map_insert(struct hash_map *hash_map, const char *key, char *value,
                     bool *updated)
{
    *updated = false;

    if (hash_map == NULL || hash_map->size == 0)
    {
        *updated = false;
        return false;
    }

    size_t hash_val = hash(key);
    struct pair_list *pair = calloc(1, sizeof(struct pair_list));
    pair->next = NULL;

    if (hash_val >= hash_map->size)
    {
        hash_val = hash_val % hash_map->size;
    }
    if (pair == NULL)
    {
        //*updated = false;
        return false;
    }

    pair->value = value;
    pair->key = key;

    if (hash_map->data[hash_val] == NULL)
    {
        hash_map->data[hash_val] = pair;
        *updated = false;
        return true;
    }
    else
    {
        struct pair_list *tmp = hash_map->data[hash_val];
        // do that in while loop
        if (strcmp(tmp->key, key) == 0)
        {
            tmp->value = value;
            *updated = true;
            free(pair);
            return true;
        }
        *updated = false;

        while (tmp != NULL)
        {
            if (strcmp(tmp->key, key) == 0)
            {
                tmp->value = value;
                *updated = true;
                break;
            }
            tmp = tmp->next;
        }

        if (!(*updated))
        {
            pair->next = hash_map->data[hash_val];
            // hash_map->data[hash_val]->next = pair;
            hash_map->data[hash_val] = pair;
            return true;
        }

        free(pair);
    }

    return true;
}

int hm_get_int(const struct hash_map *h, const char *key)
{
    const char *res = hash_map_get(h, key);
    if (res == NULL)
        return -1;
    return atoi(res);
}

const char *hash_map_get(const struct hash_map *hash_map, const char *key)
{
    size_t hash_val = hash(key);

    if (hash_map == NULL)
    {
        return NULL;
    }

    if (hash_map->size == 0)
    {
        return NULL;
    }

    if (hash_val >= hash_map->size)
    {
        hash_val = hash_val % hash_map->size;
    }

    struct pair_list *pairs = hash_map->data[hash_val];
    struct pair_list *tmp = pairs;

    while (tmp && strcmp(key, tmp->key) != 0)
    {
        tmp = tmp->next;
    }

    if (tmp == NULL)
    {
        return NULL;
    }

    const char *res = tmp->value;
    return res;
}

bool hash_map_remove(struct hash_map *hash_map, const char *key)
{
    if (hash_map == NULL)
    {
        return false;
    }

    if (hash_map->size == 0)
    {
        return false;
    }
    size_t hash_val = hash(key);

    if (hash_val >= hash_map->size)
    {
        hash_val = hash_val % hash_map->size;
    }

    struct pair_list *pairs = hash_map->data[hash_val];
    if (pairs == NULL)
    {
        return false;
    }

    struct pair_list *tmp = pairs;

    if (strcmp(key, tmp->key) == 0)
    {
        hash_map->data[hash_val] = tmp->next;
        free(tmp);
        return true;
    }
    else
    {
        while (tmp->next && (strcmp(key, tmp->next->key) != 0))
        {
            tmp = tmp->next;
        }

        if (tmp->next)
        {
            struct pair_list *to_remove = tmp->next;
            tmp->next = to_remove->next;
            free(to_remove);
            return true;
        }
        return false;
    }

    return false;
}

/*changed : freed list -> value*/
void list_destroy(struct pair_list *list)
{
    if (!list->next)
    {
        free(list -> value);
        free(list);
        list = NULL;
    }
    else
    {
        list_destroy(list->next);
        free(list->value);
        free(list);
    }
}

void hash_map_free(struct hash_map *hash_map)
{
    if (hash_map == NULL)
    {
        return;
    }
    for (size_t i = 0; i < hash_map->size; i++)
    {
        if (hash_map->data[i] != NULL)
        {
            list_destroy(hash_map->data[i]);
        }
    }

    free(hash_map->data);
    free(hash_map);
}

void hash_map_dump(struct hash_map *hash_map)
{
    for (size_t i = 0; i < hash_map->size; i++)
    {
        if (hash_map->data[i] != NULL)
        {
            struct pair_list *tmp = hash_map->data[i];

            while (tmp->next)
            {
                printf("%s: %s, ", tmp->key, tmp->value);
                tmp = tmp->next;
            }

            printf("%s: %s", tmp->key, tmp->value);
            printf("\n");
        }
    }
}
