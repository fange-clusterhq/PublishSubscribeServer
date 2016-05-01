#pragma once

#include <set>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>

static const int MAX_NUM_CONNECTION = 3;
static const int MAX_MSG_BUFFER_SIZE = 4096;

class Server {
   public:
      Server(int port);
      ~Server();

      void Init();
      void Start();
   private:
      int port;
      fd_set readfds;
      fd_set writefds;
      int masterSocket;
      std::set<int> clientFds;

      void AcceptConnection();
      void HandleRequest(int clientFd);
      int PrepareSelect();
};
