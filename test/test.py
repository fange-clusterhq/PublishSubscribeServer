from client import Client
import time

port = 10000

def ContentTest():
    # Testing the server with POST request with content
    client = Client(port)
    client.connect()
    msg = "POST /CMU/yihuaf HTTP/1.0\r\n\r\n"
    client.send(msg)
    time.sleep(1)
    msg = "POST /CMU HTTP/1.0\r\nContent-Length: 5\r\n\r\nHello"
    client.send(msg)
    msg = "POST /CMU HTTP/1.0\r\nContent-Length: 5\r\n\r\nWorld"
    client.send(msg)
    client.close()

def BasicHttpServerTest():
    msg = "GET /abcd/egs HTTP/1.0\r\n\r\n"
    client = Client(port)
    client.connect()
    client.send(msg)
    time.sleep(1)
    client.send(msg)
    client.close()
    return

def Test():
    #BasicHttpServerTest()
    ContentTest()
    return

if __name__ == "__main__":
    Test()
