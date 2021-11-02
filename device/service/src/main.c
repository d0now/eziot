#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#include "proc.h"

#define CLOUD_IP   "172.20.0.2"
#define CLOUD_PORT 54321

typedef struct __attribute__((__packed__)) __peer {
    char header[4];
    uint16_t pid;
    uint32_t addr;
    uint16_t port;
} peer_t;

typedef struct __client {
    int host;
    short port;
} client_t;

pid_t spawn_proc(void)
{
    pid_t pid = fork();
    if (pid < 0)
        return -1;
    else if (pid != 0)
        return pid;

    proc();
    exit(0);
}

int main(void)
{
    pid_t childs[5];

    setvbuf(stdout, NULL, 2, 0);
    setvbuf(stderr, NULL, 2, 0);

    for (int i = 0; i < 5; i++)
    {
        pid_t child = spawn_proc();
        if (child < 0) {
            fprintf(stderr, "Can't spawn child process!\n");
            return -1;
        }

        childs[i] = child;
    }

    while (true)
    {
        for (int i = 0; i < 5; i++)
        {
            int status;
            pid_t child = childs[i];
            if (waitpid(child, &status, WNOHANG) < 0) {
                child = spawn_proc();
                childs[i] = child;
            }
        }

        sleep(1);
    }

    return 0;
}