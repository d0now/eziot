#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "peer.h"

Peer *peer_ctor(char *cloud_ip, unsigned short cloud_port)
{    
    Peer *peer = (Peer *)(malloc(sizeof(Peer)));
    if (peer == NULL)
        return NULL;

    int udp;
    if ((udp = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        goto fail_after_alloc;

    struct sockaddr_in cloud_addr;
    memset(&cloud_addr, 0, sizeof(struct sockaddr_in));
    cloud_addr.sin_family = AF_INET;
    cloud_addr.sin_port = htons(cloud_port);
    if (inet_aton(cloud_ip, &cloud_addr.sin_addr) == 0)
        goto fail_after_alloc;

    peer->udp = udp;
    memcpy(&peer->cloud_addr, &cloud_addr, sizeof(struct sockaddr_in));
    memset(&peer->peer_addr, 0, sizeof(struct sockaddr_in));

    return peer;

fail_after_alloc:
    free(peer);
    return NULL;
}

int peer_assign(Peer *peer, char *secret)
{
    packet_assign_t pkt_assign;
    memcpy(&pkt_assign.header, "ASGN", 4);
    memcpy(&pkt_assign.secret, secret, 16);

    int sendto_result = sendto(peer->udp, &pkt_assign, sizeof(packet_assign_t), 0,
                               (struct sockaddr *)(&peer->cloud_addr), sizeof(struct sockaddr_in));
    
    if (sendto_result < 0)
        return -1;
    else
        return 0;
}

int peer_connect(Peer *peer)
{
    packet_peer_t pkt_peer;
    memset(&pkt_peer, 0, sizeof(pkt_peer));

    struct sockaddr_in saddr = {0, };
    socklen_t slen = sizeof(struct sockaddr_in);

    int recvfrom_result = recvfrom(peer->udp, &pkt_peer, sizeof(pkt_peer), 0,
                                   (struct sockaddr *)(&saddr), &slen);

    if (recvfrom_result != sizeof(pkt_peer))
        return -1;

    if (memcmp(pkt_peer.header, "PEER", 4))
        return -1;

    if (strcmp(inet_ntoa(saddr.sin_addr), inet_ntoa(peer->cloud_addr.sin_addr)))
        return -1;

    if (saddr.sin_addr.s_addr != peer->cloud_addr.sin_addr.s_addr)
        return -1;

    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = pkt_peer.addr;
    saddr.sin_port = pkt_peer.port;

    int sendto_result = sendto(peer->udp, "HELO", 4, 0, (struct sockaddr *)(&saddr), sizeof(saddr));
    if (sendto_result < 0)
        return -1;

    char buf[4];
    slen = sizeof(struct sockaddr_in);
    recvfrom_result = recvfrom(peer->udp, buf, 4, 0, (struct sockaddr *)(&saddr), &slen);
    if (recvfrom_result != 4)
        return -1;

    if (saddr.sin_port != pkt_peer.port || saddr.sin_addr.s_addr != pkt_peer.addr)
        return -1;

    if (memcmp(buf, "HELO", 4))
        return -1;

    peer->peer_addr.sin_family = AF_INET;
    peer->peer_addr.sin_addr.s_addr = pkt_peer.addr;
    peer->peer_addr.sin_port = pkt_peer.port;
    return 0;
}

int peer_recv(Peer *peer, void *buf, size_t len)
{
    struct sockaddr_in saddr;
    socklen_t slen = sizeof(saddr);

    int recvfrom_result = recvfrom(peer->udp, buf, len, 0,
                                   (struct sockaddr *)(&saddr), &slen);

    if (recvfrom_result < 0)
        return -1;

    if (saddr.sin_addr.s_addr != peer->peer_addr.sin_addr.s_addr)
        return -1;
    
    if (saddr.sin_port != peer->peer_addr.sin_port)
        return -1;

    return recvfrom_result;
}