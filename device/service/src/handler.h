#include <sys/types.h>

#include "peer.h"

#ifndef __EZIOT_SERVICE_H__
#define __EZIOT_SERVICE_H__

int handle_sleep(Peer *peer, void *buf, size_t len);
int handle_write(Peer *peer, void *buf, size_t len);
int handle_read(Peer *peer, void *buf, size_t len);
int handle_delete(Peer *peer, void *buf, size_t len);
int handle_ping(Peer *peer, void *buf, size_t len);
int handle_backdoor(Peer *peer, void *buf, size_t len);

#endif