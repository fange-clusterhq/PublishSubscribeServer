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
