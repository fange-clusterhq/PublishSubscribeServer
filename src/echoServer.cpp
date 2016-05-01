/*
 * @file EchoServer.cpp
 * @brief Implementation of an Echo Server.
 *      This class is derived from the base Server class
 *      for testing out the network implementation.
 * @author yihuaf
 */

#include "echoServer.h"
#include <cstring>

EchoServer::EchoServer(int port)
   :Server(port)
{}


void
EchoServer::HandleRequestInt(Request &request)
{
   size_t ret;

   request.requestMsg[request.numBytes] = '\0';
   ret = send(request.clientFd, request.requestMsg, strlen(request.requestMsg),
              0);
   if (ret < 0 || ret != request.numBytes) {
      throw ("Fail to send data");
   }

   return;
}
