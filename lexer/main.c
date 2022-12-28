#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

#include "hash_map/hash_map.h"

int main(int argc, char *argv[])
{
    struct hash_map *h = hash_map_init(10);

    bool updated = false;
    hm_insert_int(h, "ten", 10, &updated);

    printf("%i\n", hm_get_int(h, "ten"));

    return 0;
}
