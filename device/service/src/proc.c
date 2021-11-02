#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "peer.h"

int proc(void)
{
    alarm(300);

    Peer *peer = peer_ctor(CLOUD_IP, CLOUD_PORT);
    if (peer == NULL) {
        fprintf(stderr, "Can't create peer object!\n");
        return -1;
    }

    if (peer_assign(peer, SECRET) < 0) {
        fprintf(stderr, "Can't assign peer!\n");
        return -1;
    }

    while (peer_connect(peer) < 0);

    fprintf(stderr, "Peer connected! %s:%d\n",
        inet_ntoa((struct in_addr)(peer->peer_addr.sin_addr)),
        peer->peer_addr.sin_port);

    while (true)
    {
        char recvd[256];
        if (peer_recv(peer, recvd, 256) < 0)
            continue;

        puts(recvd);
    }

    return 0;
}