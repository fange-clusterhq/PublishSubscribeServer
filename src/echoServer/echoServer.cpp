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
   size_t ret = 0;
   int flag = 0;

   request.requestMsg[request.numBytes] = '\0';
   printf("Received msg: %s\n", request.requestMsg);
   /* SIGEPIPE should not kill the server. */
   flag = MSG_NOSIGNAL;
   ret = send(request.clientFd, request.requestMsg, strlen(request.requestMsg),
              flag);
   if (ret < 0 || ret != request.numBytes) {
      throw ("Fail to send data");
   }

   return;
}
