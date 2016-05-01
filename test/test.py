from client import Client

port = 10000

def EchoServerTest():
    msg = "This message should be repeated"
    client = Client(port)
    client.connect()
    client.send(msg)
    recv_msg = client.recv(len(msg))
    print recv_msg
    if msg != recv_msg:
        raise Exception("Failed EchoServer Test")
    client.close()
    return

def Test():
    EchoServerTest()
    return

if __name__ == "__main__":
    Test()
