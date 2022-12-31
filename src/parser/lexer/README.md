### Compilation (too lazy to write a Makefile..)

```
gcc lexer.c hash_map/hash_map.c hash_map/hash.c my_itoa/my_itoa.c -std=c99 -Wall -Werror -Wextra -fsanitize=address -g -D_XOPEN_SOURCE=700
```
