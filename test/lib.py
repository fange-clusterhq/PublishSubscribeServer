
__author__ = "Yihua Eric Fang (yihuaf)"

import httplib

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
   uri = "/%s" % (topic)
   conn.request("POST", uri, body = content)
   r = conn.getresponse()
   print r.status, r.reason
   return r.status

def Subscribe(conn, topic, username):
   """ Wrapper to issue a Subscribe command.
   :params conn The connection/client.
   :params topic The topic.
   :params username The username.
   """
   uri = "/%s/%s" % (topic, username)
   conn.request("POST", uri)
   r = conn.getresponse()
   print r.status, r.reason
   return r.status

def Unsubscribe(conn, topic, username):
   """ Wrapper to issue a Unsubscribe command.
   :params conn The connection/client.
   :params topic The topic.
   :params username The username.
   """
   uri = "/%s/%s" % (topic, username)
   conn.request("DELETE", uri)
   r = conn.getresponse()
   print r.status, r.reason
   return r.status

def GetNext(conn, topic, username):
   """ Wrapper to issue a GetNext command.
   :params conn The connection/client.
   :params topic The topic.
   :params username The username.
   """
   uri = "/%s/%s" % (topic, username)
   conn.request("GET", uri)
   r = conn.getresponse()
   print r.status, r.reason
   print r.getheaders()
   return r.status

func_map = {"Subscribe" : Subscribe,
            "Unsubscribe" : Unsubscribe,
            "Publish" : Publish,
            "GetNext" : GetNext}

