#pragma once

#include "server.h"

class EchoServer : public Server {
   public:
      explicit EchoServer(int port);
   private:
      void HandleRequestInt(Request &request);
};
