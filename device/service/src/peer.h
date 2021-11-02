#include <stdint.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#ifndef __EZIOT_SERVICE_PEER_H__
#define __EZIOT_SERVICE_PEER_H__

#define CLOUD_IP   "172.20.0.2"
#define CLOUD_PORT 54321

#ifndef SECRET
#define SECRET "RANDOMSECRETHERE"
#endif

typedef struct __attribute__((__packed__)) packet_assign {
    char header[4];
    char secret[16];
} packet_assign_t;

typedef struct __attribute__((__packed__)) packet_peer {
    char header[4];
    uint16_t pid;
    uint32_t addr;
    uint16_t port;
} packet_peer_t;

typedef struct __peer {
    int udp;
    struct sockaddr_in cloud_addr;
    struct sockaddr_in peer_addr;
} Peer;

Peer *peer_ctor(char *cloud_ip, unsigned short cloud_port);
int peer_assign(Peer *peer, char *secret);
int peer_connect(Peer *peer);
int peer_recv(Peer *peer, void *buf, size_t len);

#endif