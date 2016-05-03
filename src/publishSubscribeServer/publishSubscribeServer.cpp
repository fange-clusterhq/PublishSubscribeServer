/*
 * @file publishSubscribeServer.cpp
 * @brief The implementation of the Publish Subscribe Server.
 * @author yihuaf
 */

#include "publishSubscribeServer.h"
#include <cassert>

using namespace std;

PublishedMsg::PublishedMsg()
   :msg(NULL),
    numBytes(0),
    refCount(0)
{}

PublishedMsg::PublishedMsg(char *msg,
                           int numBytes,
                           int refCount)
   :msg(new char[numBytes]),
    numBytes(numBytes),
    refCount(refCount)
{
   memcpy(this->msg, msg, numBytes);
}

PublishedMsg::PublishedMsg(PublishedMsg &msg)
{
   PublishedMsg(msg.msg, msg.numBytes, msg.refCount);
}

PublishedMsg::~PublishedMsg()
{
   delete msg;
}

bool
PublishedMsg::Dereference()
{
   this->refCount--;
   if (this->refCount <= 0) {
      delete msg;
      msg = NULL;
      return true;
   }

   return false;
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
   return true;
}

/*
void
PublishSubscribeServer::Subscribe(string username,
                                  string topic)
{
}

void
PublishSubscribeServer::Unsubscribe(string username,
                                    string topic)
{
}

void
PublishSubscribeServer::Publish(string topic,
                                char *msg)
{
}

void
PublishSubscribeServer::GetNextMessage(string username,
                                       string topic)
{
}
*/
