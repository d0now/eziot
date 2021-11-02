FROM alpine:3.14.2

RUN apk add --no-cache qemu-system-arm
RUN apk add --no-cache iptables

COPY ./kernel /tmp/kernel
COPY ./dtb /tmp/dtb
COPY ./initrd /tmp/initrd
COPY ./start.sh /tmp/start.sh

RUN chmod 500 /tmp/start.sh