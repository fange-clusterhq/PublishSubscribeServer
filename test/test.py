__author__ = "Yihua Eric Fang (yihuaf)"
import lib
import sys

class Test(object):
   def __init__(self):
       self.port = sys.argv[1]

   def NegativeTest(self):
       client = lib.GetClient(self.port)
       lib.Expect(lib.Unsubscribe(client, "VMWARE", "abcd"), 404);
       lib.Expect(lib.GetNext(client, "VMWARE", "abcd"), 404)
       client.close()
       return

   def PositiveTest(self):
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
       client = lib.GetClient(self.port)
       lib.Expect(lib.Subscribe(client, "CMU", "yihuaf"), 200)
       lib.Expect(lib.Publish(client, "CMU", "HELLO WORLD"), 200)
       lib.Expect(lib.GetNext(client, "CMU", "yihuaf"), 200)
       lib.Expect(lib.Unsubscribe(client, "CMU", "yihuaf"), 200)
       client.close()
       return

   def Test(self):
       self.BasicServerTest()
       self.PositiveTest()
       self.NegativeTest()
       return

if __name__ == "__main__":
    test = Test()
    test.Test()
