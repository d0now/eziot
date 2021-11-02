#!/bin/sh

if [ ! -e /tmp/network_set ]; then
    tunctl -u root -t tap0
    ifconfig tap0 192.168.100.1 up
    echo 1 > /proc/sys/net/ipv4/ip_forward
    iptables -t nat -A POSTROUTING -o eth0 -j MASQUERADE
    iptables -I FORWARD 1 -i tap0 -j ACCEPT
    iptables -I FORWARD 1 -o tap0 -m state --state RELATED,ESTABLISHED -j ACCEPT
    iptables -A INPUT -i tap0 -d 192.168.100.1 -p UDP --dport 10000:65535 -m state --state NEW -j ACCEPT
    touch /tmp/network_set
fi

stty intr '^]'

qemu-system-arm                                 \
    -M versatilepb                              \
    -m 256M                                     \
    -kernel /tmp/kernel                         \
    -initrd /tmp/initrd                         \
    -dtb /tmp/dtb                               \
    -net nic                                    \
    -net tap,ifname=tap0,script=no              \
    -nographic                                  \
    -monitor /dev/null                          \
    -append 'root=/dev/ram rdinit=/init quiet'