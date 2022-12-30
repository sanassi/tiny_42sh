#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
/*
#include "lexer/hash_map/hash_map.h"

int main(int argc, char *argv[])
{
    struct hash_map *h = hash_map_init(10);

    bool updated = false;
    hm_insert_int(h, "ten", 10, &updated);

    printf("%i\n", hm_get_int(h, "ten"));

    return 0;
}
*/


bool str_cmp(char *s1, char *s2)
{
    return strcmp(s1, s2) == 0;
}

#define BUFF_SIZE 4096

int main(int argc, char *argv[])
{
    char buff[BUFF_SIZE];
    memset(buff, 0, BUFF_SIZE);

    while (true)
    {
        printf("> ");
        fflush(stdout);
        ssize_t nread = read(0, &buff, BUFF_SIZE);
        buff[nread] = '\0';

        if (str_cmp(buff, "Q\n"))
            break;

        int pid = fork();
        if (pid == 0)
        {
            char *args[] = {"echo", buff, NULL};
            execvp("echo", args);
            errx(1, "fail!!");
        }
        else 
        {
            waitpid(pid, NULL, 0);
        }
    }


    return 0;
}
