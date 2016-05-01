import socket
import sys

port = 10000

# Create a TCP/IP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Connect the socket to the port where the server is listening
server_address = ('localhost', port)
print 'connecting to %s port %s' % (server_address, port)
sock.connect(server_address)
try:
   # Send data
   message = 'This is the message.  It will be repeated.'
   print 'sending "%s"' % message
   sock.sendall(message)
   # Look for the response
   amount_received = 0
   amount_expected = len(message)
   data_received = ""
   while amount_received < amount_expected:
      data = sock.recv(16)
      amount_received += len(data)
      data_received += data
   print 'received "%s"' % data_received

finally:
   print 'closing socket'
   sock.close()
