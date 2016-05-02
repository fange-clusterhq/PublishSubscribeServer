#pragma once

#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>

using namespace std;

static const int MAX_NUM_CONNECTION = 3;
static const int MAX_MSG_BUFFER_SIZE = 4096;

class Server {
   public:
      class Request {
         public:
            int clientFd;
            char recvBuffer[MAX_MSG_BUFFER_SIZE];
            int numBytes;

            Request();
            inline char *GetBufferForRecv();
            inline size_t GetSizeForRecv();
      };

      Server(int port);
      ~Server();

      void Init();
      void Start();
   private:
      int port;
      fd_set readfds;
      fd_set writefds;
      int masterSocket;
      map<int, Request *> clientContext;

      void AcceptConnection();
      void HandleRequest(int clientFd);
      int PrepareSelect();
      virtual bool HandleRequestInt(Request *request) = 0;
};
