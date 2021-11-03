import sys
import time
import socket
import struct
import random
import argparse
import binascii

from peer import Peer

# This method prevents anonymously connected device can't assign to p2p network :)
# But also, can't establish NAT to NAT connections :(
# Maybe next time, I'll implement those things :D
# Cheers!
DEFAULT_SECRET = b"RANDOMSECRETHERE" # 16 bytes

peers = []

def log(logstr, end='\n'):
    sys.stdout.write(logstr + end)

def get_new_pid() -> int:

    new_pid_found = False

    while not new_pid_found:

        pid = random.randint(0, 0xffff)

        if len(peers) == 0:
            new_pid_found = True

        for peer in peers:
            if peer.get_pid() == pid:
                new_pid_found = False
                break
            else:
                new_pid_found = True

    return pid

def new_peer(addr:str, port:int) -> Peer:
    global peers
    pid = get_new_pid()
    return Peer(pid, addr, port)

def find_peer(pid:int) -> Peer:
    global peers
    for peer in peers:
        if peer.get_pid() == pid:
            return peer

def find_peer_by_addr(addr:str, port:int) -> Peer:
    global peers
    for peer in peers:
        if (peer.get_addr() == addr) and (peer.get_port() == port):
            return peer

def auto_drop_peers() -> None:

    global peers

    new_peers = []

    for i in range(len(peers)):
        peer = peers[i]
        if peer.get_creat() + 15 > time.time():
            new_peers.append(peer)

    peers = new_peers

def main(host:str, port:int, secret:bytes) -> None:

    global peers

    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s.settimeout(1)
    s.bind((host, port))

    log(f"[{time.ctime()}] Starting...")

    while True:

        try:
            data, addr = s.recvfrom(32)
        except socket.timeout:
            continue
        except KeyboardInterrupt:
            exit(0)
        except Exception as e:
            log(f"[{time.ctime()}] {e}")
            break

        log(f"[{time.ctime()}] {addr[0]}:{addr[1]} => {binascii.hexlify(data[0:4]).decode()}... ", end='')

        auto_drop_peers()

        if data[0:4] == b'PING':
            continue

        elif data[0:4] == b'ASGN':

            if data[4:] != secret:
                log("=> Invalid assign request.")
                continue

            if len(peers) > 0xffff:
                log("=> No more space!")
                continue

            assigned_peer = find_peer_by_addr(addr[0], addr[1])
            if assigned_peer:
                assigned_peer.alive()
                log("=> Peer already assigned!")
                continue

            peer = new_peer(addr[0], addr[1])
            peers.append(peer)
            log(f"=> Peer assigned. ({peer.get_pid()})")
            continue

        elif data[0:4] == b'CONN':
            
            if len(data[4:]) != 2:
                log("=> Invalid connect request.")
                continue

            if len(peers) < 1:
                log("=> No peers!")
                continue

            pid = struct.unpack("<H", data[4:])[0]
            peer = find_peer(pid)
            if peer == None:
                log(f"=> Can't find peer! {pid}")
                continue

            to_send = Peer(0, addr[0], addr[1]).pack()
            log(f"=> {to_send} ", end='')
            s.sendto(to_send, peer.addr())

            to_send = peer.pack()
            log(f"=> {to_send} ", end='')
            s.sendto(to_send, addr)

            log(f"=> Connect request handled.")
            continue

        elif data[0:4] == b'LIST':
            for peer in peers: # This method needs to send extremely big data when many peers assigned. need fix?
                s.sendto(peer.pack(), addr)
            log(f"=> Sent list of peers assigned ^^7")
            continue

        else:
            log("=> Unknown...")
            continue

        log("")

if __name__ == '__main__':
    
    parser = argparse.ArgumentParser()
    parser.add_argument("host", type=str)
    parser.add_argument("port", type=int)
    parser.add_argument("--secret", type=str)
    args = parser.parse_args()

    if args.secret:
        secret = binascii.unhexlify(args.secret)
    else:
        secret = DEFAULT_SECRET

    while True:
        try:
            main(args.host, args.port, secret)
        except KeyboardInterrupt:
            exit(0)
        except Exception as e:
            log(f"[{time.ctime()}] main() exception occured {e}")