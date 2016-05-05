import lib

port = 10000

def ContentTest():
    # Testing the server with POST request with content
    client = lib.GetClient(port)
    lib.Subscribe(client, "CMU", "yihuaf")
    lib.Publish(client, "CMU", "HELLO WORLD")
    lib.GetNext(client, "CMU", "yihuaf")
    client.close()

def BasicHttpServerTest():
    client = lib.GetClient(port)
    client.close()
    return

def Test():
    #BasicHttpServerTest()
    ContentTest()
    return

if __name__ == "__main__":
    Test()
