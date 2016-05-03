/*
 * @file publishSubscribeServer.cpp
 * @brief The implementation of the Publish Subscribe Server.
 * @author yihuaf
 */

#include "publishSubscribeServer.h"

using namespace std;

PublishSubscribeServer::PublishSubscribeServer(int port)
   :Server(port)
{}

PublishSubscribeServer::~PublishSubscribeServer()
{
}

bool
PublishSubscribeServer::HandleRequestInt(Request *request)
{
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
