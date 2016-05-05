from client import Client

port = 10000

def BasicHttpServerTest():
    msg = "GET /abcd/egs HTTP/1.0\r\n\r\n"
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
    BasicHttpServerTest()
    return

if __name__ == "__main__":
    Test()
