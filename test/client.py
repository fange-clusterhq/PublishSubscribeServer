import socket
import sys

class Client(object):
    def __init__(self, port):
        self.port = port
        # Create a TCP/IP socket
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        return

    def connect(self):
        server_address = ('localhost', self.port)
        print 'connecting to %s port %s' % (server_address, self.port)
        self.sock.connect(server_address)
        return

    def send(self, message):
        print 'sending "%s"' % message
        self.sock.sendall(message)
        return

    def recv(self, expected):
        # Look for the response
        amount_received = 0
        amount_expected = expected
        data_received = ""
        while amount_received < amount_expected:
           data = self.sock.recv(4096)
           amount_received += len(data)
           data_received += data
        print 'received "%s"' % data_received
        return data_received

    def close(self):
        print 'closing socket'
        self.sock.close()

