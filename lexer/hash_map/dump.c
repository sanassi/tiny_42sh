#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hash_map.h"
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
