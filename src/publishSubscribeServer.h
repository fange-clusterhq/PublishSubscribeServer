#pragma once

#include "server.h"

class PublishSubscribeServer : public Server {
   public:
      explicit PublishSubscribeServer(int port);
      ~PublishSubscribeServer();
   private:
      void HandleRequestInt(Request &request);
};
