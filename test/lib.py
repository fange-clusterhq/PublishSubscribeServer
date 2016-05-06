
__author__ = "Yihua Eric Fang (yihuaf)"

import httplib
import socket
import sys

def Expect(val, expected):
   """ Wrapper for the assert used in testing.
   :params val The input status code.
   :params expected The expected value.
   """
   if val != expected :
      print val, expected
   assert(val == expected)

def GetClient(port):
   """ Create a client.
   :params port The port to connect to.
   """
   client = httplib.HTTPConnection("localhost", port)
   return client

def Publish(conn, topic, content):
   """ Wrapper to issue a Publish Command.
   :params conn The connection/client.
   :params topic The topic.
   :params content The published content.
   """
   print "Publish"
   conn.connect()
   uri = "/%s" % (topic)
   conn.request("POST", uri, body = content)
   r = conn.getresponse()
   print r.status, r.reason
   conn.close()
   return r.status

def Subscribe(conn, topic, username):
   """ Wrapper to issue a Subscribe command.
   :params conn The connection/client.
   :params topic The topic.
   :params username The username.
   """
   print "Subscribe"
   conn.connect()
   uri = "/%s/%s" % (topic, username)
   conn.request("POST", uri)
   r = conn.getresponse()
   print r.status, r.reason
   conn.close()
   return r.status

def Unsubscribe(conn, topic, username):
   """ Wrapper to issue a Unsubscribe command.
   :params conn The connection/client.
   :params topic The topic.
   :params username The username.
   """
   print "Unsubsribe"
   conn.connect()
   uri = "/%s/%s" % (topic, username)
   conn.request("DELETE", uri)
   r = conn.getresponse()
   print r.status, r.reason
   conn.close()
   return r.status

def GetNext(conn, topic, username):
   """ Wrapper to issue a GetNext command.
   :params conn The connection/client.
   :params topic The topic.
   :params username The username.
   """
   print "GetNext"
   conn.connect()
   uri = "/%s/%s" % (topic, username)
   conn.request("GET", uri)
   r = conn.getresponse()
   print r.status, r.reason
   if r.status == 200:
      print r.read()
   conn.close()
   return r.status

func_map = {"Subscribe" : Subscribe,
            "Unsubscribe" : Unsubscribe,
            "Publish" : Publish,
            "GetNext" : GetNext}

class RawClient(object):
    """ A client that can sent raw http request.

    One of the limitations of using httplib is that we cannot test malformed
    http request to the server. This client allows the user to send http
    request in the form of raw string and testing out edge cases.
    """
    def __init__(self, port):
        self.port = int(port)
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

def GenerateHttpRequest(method, uri):
    return "%s %s HTTP/1.0\r\n\r\n" % (method, uri)
