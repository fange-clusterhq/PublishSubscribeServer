#pragma once

#include <server/server.h>

class PublishSubscribeServer : public Server {
   public:
      explicit PublishSubscribeServer(int port);
      ~PublishSubscribeServer();
   private:
      void HandleRequestInt(Request &request);
      /*
      void Subscribe();
      void Unsubscribe();
      void Publish();
      void GetNextMessage();
      */
};
