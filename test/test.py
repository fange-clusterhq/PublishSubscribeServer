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
       client = lib.RawClient(self.port)
       client.connect()
       for i in range(0, 100):
          client.send(lib.GenerateHttpRequest("DELETE", "/CMU/yihuaf"))

       client.close()
       return

   def EdgeCaseTest(self):
       # Test for some edge cases.
       # Since the server will drop the request that is ill formed,
       # we will not be expecting a response.
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
       client.close()
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
       client.close()
       return

   def BasicServerTest(self):
       # Try all operation once.
       client = lib.GetClient(self.port)
       lib.Expect(lib.Subscribe(client, "CMU", "yihuaf"), 200)
       lib.Expect(lib.Publish(client, "CMU", "HELLO WORLD"), 200)
       lib.Expect(lib.GetNext(client, "CMU", "yihuaf"), 200)
       lib.Expect(lib.Unsubscribe(client, "CMU", "yihuaf"), 200)
       client.close()
       return

   def Test(self):
       #self.BasicServerTest()
       #self.PositiveTest()
       #self.NegativeTest()
       #self.FlushingTest()
       #self.EdgeCaseTest()
       self.PartialTest()
       return

if __name__ == "__main__":
    test = Test()
    test.Test()
