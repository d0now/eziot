import struct
import socket
import time

class Peer:

    def __init__(self, pid:int, addr:str, port:int):

        if pid < 0 or pid > 0xffff:
            raise RuntimeError("pid is not short range (0~0xffff)")

        if port < 0 or port > 0xffff:
            raise RuntimeError("pid is not short range (0~0xffff)")

        self.__pid   = pid
        self.__addr  = addr
        self.__port  = port
        self.__creat = time.time()

    # def unpack(data:bytes) -> Peer:
    #
    #     if len(data) != 8:
    #         raise ValueError("'data' must be size of 8")
    #
    #     pid, addr, port = struct.unpack("<HLH", data)
    #     return Peer(pid, addr, port)

    def pack(self) -> bytes:
        ret  = b'PEER'
        ret += struct.pack("<H", self.get_pid())
        ret += self.get_addr_n()
        ret += struct.pack(">H", self.get_port())
        return ret

    def addr(self) -> bytes:
        return (self.get_addr(), self.get_port())

    def get_pid(self) -> int:
        return self.__pid

    def get_addr(self) -> str:
        return self.__addr

    def get_addr_n(self) -> int:
        return socket.inet_aton(self.__addr)

    def get_port(self) -> int:
        return self.__port

    def get_creat(self) -> int:
        return self.__creat

    def alive(self) -> None:
        self.__creat = time.time()