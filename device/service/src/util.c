#include <string.h>
#include <stdbool.h>
#include <stdint.h>

int sanitize_path(char *path, char *dest)
{
    size_t len = strlen(path);

    for (int i = 0; i < len; i++)
    {
        if (path[i] != '.')
            *(dest++) = path[i];
    }

    *dest = '\0';

    return 0;
}

int check_ipaddr(char *ipaddr)
{
    while (*ipaddr != '\0')
    {
        char now = *ipaddr++;
        if ((now < '0' || now > '9') && (now != '.'))
            return -1;
    }

    return 0;
}