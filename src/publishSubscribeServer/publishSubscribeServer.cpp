/*
 * @file publishSubscribeServer.cpp
 * @brief The implementation of the Publish Subscribe Server.
 * @author yihuaf
 */

#include "publishSubscribeServer.h"
#include "publishSubscribeRequest.h"
#include <cassert>

using namespace std;

PublishedMsg::PublishedMsg()
   :msg(),
    refCount(0)
{}

PublishedMsg::PublishedMsg(string &msg,
                           int refCount)
   :msg(msg),
    refCount(refCount)
{}

PublishedMsg::PublishedMsg(PublishedMsg &msg)
{
   PublishedMsg(msg.msg, msg.refCount);
}

bool
PublishedMsg::Dereference()
{
   this->refCount--;
   return (this->refCount <= 0);
}

PublishSubscribeServer::PublishSubscribeServer(int port)
   :Server(port)
{}

PublishSubscribeServer::~PublishSubscribeServer()
{
}

bool
PublishSubscribeServer::HandleRequestInt(ReadRequest *request)
{
   assert(request != NULL);

   string httpRequest = string(request->buffer);
   PublishSubscribeRequest psRequest;
   psRequest.Translate(httpRequest);
   int statusCode;
   string msgOut;
   switch(psRequest.opCode) {
      case PublishSubscribeServerOp::SUBSCRIBE:
         statusCode = this->Subscribe(psRequest.username, psRequest.topic);
         break;
      case PublishSubscribeServerOp::UNSUBSCRIBE:
         statusCode = this->Unsubscribe(psRequest.username, psRequest.topic);
         break;
      case PublishSubscribeServerOp::PUBLISH:
         statusCode = this->Publish(psRequest.topic, psRequest.msg);
         break;
      case PublishSubscribeServerOp::GET_NEXT_MSG:
         statusCode = this->GetNextMessage(psRequest.topic, psRequest.username,
                                           msgOut);
         break;
      case PublishSubscribeServerOp::CONTINUE:
         return false;
      default:
         /* In error case, we simply drop the meesage and fake success. */
         return true;
   }

   /* Queue up the response. */
   WriteRequest *response = new WriteRequest();
   response->clientFd = request->clientFd;
   string httpResponse = PublishSubscribeResponse::FormResponse(statusCode,
                                                                msgOut);
   /* Needs to copy the '\0' at the end, so size + 1. */
   memcpy(response->buffer, httpResponse.c_str(), httpResponse.size() + 1);
   response->numBytes = httpResponse.size() + 1;
   this->queueMsg(response->clientFd, response);
   return true;
}

int
PublishSubscribeServer::Subscribe(const string &username,
                                  const string &topic)
{
   assert(!username.empty());
   assert(!topic.empty());

   this->topicSubscription[topic].insert(username);
   return OK;
}

int
PublishSubscribeServer::Unsubscribe(const string &username,
                                    const string &topic)
{
   assert(!username.empty());
   assert(!topic.empty());

   int ret = OK;
   auto it = this->topicSubscription.find(topic);
   if (it != this->topicSubscription.end()) {
      it->second.erase(username);
      if (it->second.size() == 0) {
         this->topicSubscription.erase(it);
      }
   } else {
      ret = NOT_FOUND;
   }

   return ret;
}

int
PublishSubscribeServer::Publish(const string &topic,
                                string &msg)
{
   assert(!topic.empty());
   assert(!msg.empty());

   /*
    * If there is no user subscribe to the topic, then we don't need to publish
    * this. We can fake OK.
    */
   auto it = this->topicSubscription.find(topic);
   if (it != this->topicSubscription.end()) {
      set<string> &usernames = it->second;
      PublishedMsg *publishedMsg = new PublishedMsg(msg, usernames.size());
      printf("Published '%s' to topic '%s'\n", msg.c_str(), topic.c_str());
      for(auto &user : usernames) {
         pair<string, string> msgQueueKey(user, topic);
         this->msgQueue[msgQueueKey].push(publishedMsg);
      }
   }

   return OK;
}


int
PublishSubscribeServer::GetNextMessage(const string &username,
                                       const string &topic,
                                       string &msgOut)
{
   assert(!username.empty());
   assert(!topic.empty());

   /*
    * Find out if the username subscribed to the topic.
    * Either the topic has no subscriber or the user is not on the subscriber
    * list.
    */
   auto topicSubscriptionIt = this->topicSubscription.find(topic);
   if (topicSubscriptionIt == this->topicSubscription.end()) {
      return NOT_FOUND;
   } else {
      auto usernamesIt = topicSubscriptionIt->second.find(username);
      if (usernamesIt == topicSubscriptionIt->second.end()) {
         return NOT_FOUND;
      }
   }

   /* Check if there is any messages on the topic for the user. */
   pair<string, string> msgQueueKey(username, topic);
   auto it = this->msgQueue.find(msgQueueKey);
   if (it != this->msgQueue.end()) {
      PublishedMsg *msg = it->second.front();
      it->second.pop();
      msgOut = msg->msg;
      if (msg->Dereference()) {
         delete msg;
      }
   } else {
      return NO_CONTENT;
   }

   return OK;
}
