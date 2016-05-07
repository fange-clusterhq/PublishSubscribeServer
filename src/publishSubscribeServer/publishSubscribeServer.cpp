/*
 * @file publishSubscribeServer.cpp
 * @brief The implementation of the Publish Subscribe Server.
 * @author yihuaf
 */

#include "publishSubscribeServer.h"
#include "publishSubscribeRequest.h"
#include <cassert>
#include <cstring>

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
   for (auto it = this->msgQueue.begin(); it != msgQueue.end(); it++) {
      auto &queue = it->second;
      if (queue.empty()) {
         continue;
      }

      while (!queue.empty()) {
         if (queue.front() != NULL && queue.front()->Dereference()) {
            delete queue.front();
         }

         queue.pop();
      }
   }
}


bool
PublishSubscribeServer::HandleRequestInt(ReadRequest *request)
{
   assert(request != NULL);
   for(;;) {
      assert(request->numBytes > 0);

      /*
       * Translate the received request as http request and into operations
       * this server supports.
       */
      string httpRequest = string(request->buffer);
      PublishSubscribeRequest psRequest;
      size_t bytesConsumed = psRequest.Translate(httpRequest);
      int statusCode;
      string msgOut;
      /* Dispatching based on parsed op code. */
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
            statusCode = this->GetNextMessage(psRequest.username,
                                              psRequest.topic, msgOut);
            break;
         case PublishSubscribeServerOp::CONTINUE:
            /*
             * If it is a partial request, we need to leave the request
             * unchanged and return the status back to the caller. The
             * only exception will be if the buffer has already reached
             * the max. Then we treat the this case as an error.
             */
            if (request->numBytes < MAX_MSG_BUFFER_SIZE) {
               return false;
            } else{
               statusCode = BAD_REQUEST;
            }
         case PublishSubscribeServerOp::ERROR:
            statusCode = BAD_REQUEST;
            break;
         default:
            assert(false);
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

      /* Remove the processed bytes from the request. */
      request->Consume(bytesConsumed);
      if (request->numBytes == 0) {
         break;
      }
   }

   return true;
}


int
PublishSubscribeServer::Subscribe(const string &username,
                                  const string &topic)
{
   assert(!username.empty());
   assert(!topic.empty());

   printf("[Subscribe] Topic: %s Username: %s\n", topic.c_str(),
          username.c_str());
   this->topicSubscription[topic].insert(username);
   return OK;
}


int
PublishSubscribeServer::Unsubscribe(const string &username,
                                    const string &topic)
{
   assert(!username.empty());
   assert(!topic.empty());

   printf("[Unsubscribe] Topic: %s Username: %s\n", topic.c_str(),
          username.c_str());
   int ret = OK;
   auto it = this->topicSubscription.find(topic);
   if (it != this->topicSubscription.end()) {
      it->second.erase(username);
      /* If no user subscribe to the topic after unsubscribe, we erase the list
       * from the map.
       */
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
   printf("[Publish] Topic: %s Content: %s\n", topic.c_str(),
          msg.c_str());

   /*
    * If there is no user subscribe to the topic, then we don't need to publish
    * this. We can fake OK.
    */
   auto it = this->topicSubscription.find(topic);
   if (it != this->topicSubscription.end()) {
      set<string> &usernames = it->second;
      PublishedMsg *publishedMsg = new PublishedMsg(msg, usernames.size());
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

   printf("[GetNext] Topic: %s Username: %s\n", topic.c_str(),
          username.c_str());
   /*
    * Find out if the username subscribed to the topic.  Either the topic has
    * no subscriber or the user is not on the subscriber list.
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
         /* Safe to delete the msg if the ref count reaches 0. */
         delete msg;
      }

      if (it->second.empty()) {
         this->msgQueue.erase(it);
      }
   } else {
      return NO_CONTENT;
   }

   printf("[GetNext] Out content: %s\n", msgOut.c_str());
   return OK;
}
