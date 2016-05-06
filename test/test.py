__author__ = "Yihua Eric Fang (yihuaf)"
import lib
import sys
import time

class Test(object):
   def __init__(self):
       self.port = sys.argv[1]
       return

   def PartialTest(self):
       # Sending valid request but divide them into multiple send.
       client = lib.RawClient(self.port)
       client.connect()
       msg = lib.GenerateHttpRequest("POST", "/VMWARE/yihuaf");
       client.send(msg[:5])
       # Sleep to force the request are not sent in one receive.
       time.sleep(2)
       client.send(msg[5:])
       client.close()
       return

   def FlushingTest(self):
       # Flood the server with a lot of request and make sure
       # the server does not crash.
       client = lib.RawClient(self.port)
       client.connect()
       for i in range(0, 50):
          client.send(lib.GenerateHttpRequest("DELETE", "/CMU/yihuaf"))

       client.close()
       return

   def EdgeCaseTest(self):
       # Test for some edge cases.
       # Make sure these requests does not crash the server.
       client = lib.RawClient(self.port)
       client.connect()
       client.send(lib.GenerateHttpRequest("GET", "///"))
       client.send(lib.GenerateHttpRequest("GET", "/VMWARE/"))
       client.send(lib.GenerateHttpRequest("GET", "//VMWARE"))
       client.send(lib.GenerateHttpRequest("POST", "/VMWARE"))
       client.send(lib.GenerateHttpRequest("POST", "/VMWARE//"))
       client.send(lib.GenerateHttpRequest("POST", "//VMWARE"))
       client.send(lib.GenerateHttpRequest("POST", "//"))
       client.send(lib.GenerateHttpRequest("POST", "/////"))
       client.send(lib.GenerateHttpRequest("DELETE", "//VMWARE"))
       client.send(lib.GenerateHttpRequest("DELETE", "/VMWARE/"))
       client.send(lib.GenerateHttpRequest("DELETE", "//"))
       client.send(lib.GenerateHttpRequest("DELETE", "/////"))
       client.close()

   def NegativeTest(self):
       # Try all the non-OK cases once.
       client = lib.GetClient(self.port)
       lib.Expect(lib.Unsubscribe(client, "VMWARE", "abcd"), 404);
       lib.Expect(lib.GetNext(client, "VMWARE", "abcd"), 404)
       # Try ill-formed request
       lib.Expect(lib.Subscribe(client, "CMU", ""), 400)
       lib.Expect(lib.Subscribe(client, "", "alice"), 400)
       lib.Expect(lib.Subscribe(client, "", ""), 400)
       lib.Expect(lib.Unsubscribe(client, "VMWARE", ""), 400);
       lib.Expect(lib.Unsubscribe(client, "", "abcd"), 400)
       lib.Expect(lib.Unsubscribe(client, "", ""), 400);
       lib.Expect(lib.GetNext(client, "CMU", ""), 400)
       lib.Expect(lib.GetNext(client, "", "alice"), 400)
       lib.Expect(lib.GetNext(client, "", ""), 400)
       lib.Expect(lib.Publish(client, "CMU", ""), 400)
       lib.Expect(lib.Publish(client, "", "alice"), 400)
       lib.Expect(lib.Publish(client, "", ""), 400)
       return

   def PositiveTest(self):
       # The test case specified in the write up.
       client = lib.GetClient(self.port)
       lib.Expect(lib.Subscribe(client, "CMU", "alice"), 200)
       lib.Expect(lib.Subscribe(client, "CMU", "bob"), 200)
       lib.Expect(lib.Publish(client, "CMU", "HELLO WORLD"), 200)
       lib.Expect(lib.GetNext(client, "CMU", "alice"), 200)
       lib.Expect(lib.GetNext(client, "CMU", "alice"), 204)
       lib.Expect(lib.GetNext(client, "CMU", "bob"), 200)
       lib.Expect(lib.Unsubscribe(client, "CMU", "yihuaf"), 200)
       return

   def BasicServerTest(self):
       # Try all operation once.
       client = lib.GetClient(self.port)
       for i in range (0, 10):
          lib.Expect(lib.Subscribe(client, "CMU", "yihuaf"), 200)
          lib.Expect(lib.Publish(client, "CMU", "HELLO WORLD"), 200)
          lib.Expect(lib.GetNext(client, "CMU", "yihuaf"), 200)
          lib.Expect(lib.Unsubscribe(client, "CMU", "yihuaf"), 200)
       return

   def Test(self):
       self.BasicServerTest()
       self.PositiveTest()
       self.NegativeTest()
       self.FlushingTest()
       self.EdgeCaseTest()
       self.PartialTest()
       return

if __name__ == "__main__":
    test = Test()
    test.Test()
