/*
 * @file publishSubscribeServer.cpp
 * @brief The implementation of the Publish Subscribe Server.
 * @author yihuaf
 */

#include "publishSubscribeServer.h"

PublishSubscribeServer::PublishSubscribeServer(int port)
   :Server(port)
{}

PublishSubscribeServer::~PublishSubscribeServer()
{
}

void
PublishSubscribeServer::HandleRequestInt(Request &request)
{
   return;
}

