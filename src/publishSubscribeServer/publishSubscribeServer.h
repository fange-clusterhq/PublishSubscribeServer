#pragma once

#include <server/server.h>
#include <string>
#include <map>
#include <queue>
#include "publishSubscribeRequest.h"

using namespace std;

/*
typedef struct PublishMsg {
   char *msg;
   int numBytes;
   int refCount;
} PublishMsg;

bool FreePublishMsg(PublishMsg *msg) {
   msg->refCount--;
   if (msg->refCount == 0) {
      delete msg->msg;
   }
}
*/

class PublishSubscribeSystem {
   public:
      void Subscribe(string &username, string &topic);
      void Unsubscribe(string &username, string &topic);
      void Publish(string &topic, char *msg);
      void GetNextMessage(string &username, string &topic);
   private:
      map<string, string> topicSubscription;
      map<string, queue<PublishedMsg *>> msgQueue;
};

class PublishSubscribeServer : public Server {
   public:
      explicit PublishSubscribeServer(int port);
      ~PublishSubscribeServer();
   private:
      bool HandleRequestInt(Request *request);
};
