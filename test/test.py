import lib

port = 10000

def EmptyTest():
    client = lib.GetClient(port)
    lib.Subscribe(client, "", "yihuaf")
    lib.Subscribe(client, "CMU", "")
    lib.Publish(client, "CMU", "")
    lib.GetNext(client, "CMU", "yihuaf")
    lib.Unsubscribe(client, "CMU", "yihuaf")
    client.close()

def NegativeTest():
   return

def PositiveTest():
    client = lib.GetClient(port)
    lib.Expect(lib.Subscribe(client, "CMU", "alice"), 200)
    lib.Expect(lib.Subscribe(client, "CMU", "bob"), 200)
    lib.Expect(lib.Publish(client, "CMU", "HELLO WORLD"), 200)
    lib.Expect(lib.GetNext(client, "CMU", "alice"), 200)
    lib.Expect(lib.GetNext(client, "CMU", "alice"), 204)
    lib.Expect(lib.GetNext(client, "CMU", "bob"), 200)
    lib.Expect(lib.Unsubscribe(client, "CMU", "yihuaf"), 200)
    client.close()


def BasicServerTest():
    client = lib.GetClient(port)
    lib.Expect(lib.Subscribe(client, "CMU", "yihuaf"), 200)
    lib.Expect(lib.Publish(client, "CMU", "HELLO WORLD"), 200)
    lib.Expect(lib.GetNext(client, "CMU", "yihuaf"), 200)
    lib.Expect(lib.Unsubscribe(client, "CMU", "yihuaf"), 200)
    client.close()
    return

def Test():
    BasicServerTest()
    PositiveTest()
    return

if __name__ == "__main__":
    Test()
