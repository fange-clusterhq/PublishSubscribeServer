import httplib

def Expect(val, expected):
   assert(val == expected)

def GetClient(port):
    client = httplib.HTTPConnection("localhost", port)
    return client

def Publish(conn, topic, content):
   uri = "/%s" % (topic)
   conn.request("POST", uri, body = content)
   r = conn.getresponse()
   print r.status, r.reason
   return r.status

def Subscribe(conn, topic, username):
   uri = "/%s/%s" % (topic, username)
   conn.request("POST", uri)
   r = conn.getresponse()
   print r.status, r.reason
   return r.status

def Unsubscribe(conn, topic, username):
   uri = "/%s/%s" % (topic, username)
   conn.request("DELETE", uri)
   r = conn.getresponse()
   print r.status, r.reason
   return r.status

def GetNext(conn, topic, username):
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

