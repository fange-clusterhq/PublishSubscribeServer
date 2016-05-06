/*
 * @file publishSubscribeServer/publishSubscribeServer.h
 * @brief Contains the definition of the publish subscribe server and related.
 * @author Yihua Eric Fang (yihuaf)
 */

#pragma once

#include <server/server.h>
#include <string>
#include <map>
#include <queue>
#include <set>
#include "publishSubscribeRequest.h"

using namespace std;

class PublishedMsg {
   public:
      PublishedMsg();
      /*
       * @brief Create a new published message based on msg and refCount.
       *
       * @params msg The published msg.
       * @params refCount How many refCount.
       * @return None.
       */
      PublishedMsg(string &msg, int refCount);

      /*
       * @brief The copy constrctor.
       *
       * @params msg The source msg.
       * @return None.
       */
      PublishedMsg(PublishedMsg &msg);

      /*
       * @brief Decrease the ref count of the msg.
       *
       * One of the requirement for the server is to keep the published message
       * around until everyone has received this message. Instead of maintain
       * this state in some centrolized place, we use ref count for this. When
       * the message was first pulished, we increase the ref count to the
       * number of subscriber. Each time a user get's the message, the ref
       * count is decreased. When the refcount reaches 0, it is same to delete
       * the message.
       *
       * @return True if the refCount reaches 0 and is safe to delete. False
       *         otherwise.
       */
      bool Dereference();

      /* Published Msg. */
      string msg;
      /* Set to the number of subscriber at the time. */
      int refCount;
};

class PublishSubscribeServer : public Server {
   public:
      explicit PublishSubscribeServer(int port);
      ~PublishSubscribeServer();
   private:
      /* Topic -> Usernames mapping. */
      map<string, set<string>> topicSubscription;
      /* (Username, topic) -> message mapping. */
      map<pair<string, string>, queue<PublishedMsg *>> msgQueue;

      /*
       * @brief Handle the incoming request.
       *
       * This function is the entry point into handling the incoming
       * request. It overrides the same function from the base class.
       *
       * @params request The incoming request.
       * @return True is the request is handled. False if the request
       *         is indetermined and need more.
       */
      bool HandleRequestInt(ReadRequest *request);

      /*
       * @brief Subscribe a user to a topic.
       *
       * @params username The user to be subscribed.
       * @params topic The topic to be subscribed to.
       * @return Status code.
       */
      int Subscribe(const string &username, const string &topic);

      /*
       * @brief Unsubscribe a user to a topic.
       *
       * @params username The user to be unsubscribed.
       * @params topic The topic to be unsubscribed to.
       * @return Status code.
       */
      int Unsubscribe(const string &username, const string &topic);

      /*
       * @brief Publish a message to a topic.
       *
       * @params topic The topic to be published.
       * @params msg The msg to be published.
       * @return Status code.
       */
      int Publish(const string &topic, string &msg);

      /*
       * @brief Get the next msg from the topic for the user.
       *
       * @params username The user requesting the msg.
       * @params topic The topic to get.
       * @params msgOut The next msg if present.
       * @return Status code.
       */
      int GetNextMessage(const string &username, const string &topic,
                         string &msgOut);
};
