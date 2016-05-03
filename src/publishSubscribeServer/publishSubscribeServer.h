#pragma once

#include <server/server.h>
#include <string>
#include <map>
#include <queue>
#include "publishSubscribeRequest.h"

using namespace std;

class PublishedMsg {
   public:
      PublishedMsg();
      PublishedMsg(char *msg, int numBytes, int refCount);
      PublishedMsg(PublishedMsg &msg);
      ~PublishedMsg();

      bool Dereference();

      char *msg;
      int numBytes;
      int refCount;
};

class PublishSubscribeServer : public Server {
   public:
      explicit PublishSubscribeServer(int port);
      ~PublishSubscribeServer();
   private:
      map<string, string> topicSubscription;
      map<string, queue<PublishedMsg *>> msgQueue;

      bool HandleRequestInt(ReadRequest *request);
      void Subscribe(string &username, string &topic);
      void Unsubscribe(string &username, string &topic);
      void Publish(string &topic, char *msg);
      void GetNextMessage(string &username, string &topic);
};
