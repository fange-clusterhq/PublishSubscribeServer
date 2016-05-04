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
      PublishedMsg(string &msg, int refCount);
      PublishedMsg(PublishedMsg &msg);

      bool Dereference();

      string msg;
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
      void Subscribe(const string &username, const string &topic);
      void Unsubscribe(const string &username, const string &topic);
      void Publish(const string &topic, string &msg);
      void GetNextMessage(const string &username, const string &topic);
};
