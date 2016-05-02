/*
 * @file server.cpp
 * @brief Contains the implementation of the Server base class.
 * @author yihuaf
 */

#include "server.h"
#include <cassert>
#include <cstring>

using namespace std;

Server::Request::Request(int clientFd,
                         char *requestMsg,
                         size_t numBytes)
   : clientFd(clientFd),
     requestMsg(requestMsg),
     numBytes(numBytes)
{}

Server::Server(int port)
:port(port),
 clientFds()
{}

Server::~Server() {}

void
Server::Init()
{
   struct sockaddr_in address = {0};

   if ((this->masterSocket = socket(AF_INET, SOCK_STREAM ,0)) == 0) {
      throw ("Failed to initiate the master socket");
   }

   address.sin_family = AF_INET;
   address.sin_addr.s_addr = INADDR_ANY;
   address.sin_port = htons(port);
   if (bind(this->masterSocket, (struct sockaddr *)&address,
            sizeof(address)) < 0) {
      throw ("Fail to bind");
   }

   if (listen(this->masterSocket, MAX_NUM_CONNECTION) < 0) {
      throw ("Fail to listen");
   }

   return;
}

void
Server::Start()
{
   int maxFd;
   int numReady;

   for (;;) {
      maxFd = PrepareSelect();
      numReady = select(maxFd + 1 , &this->readfds , &this->writefds, NULL,
                        NULL);
      if (numReady < 0 && errno != EINTR) {
         perror("Select error");
         continue;
      }

      if (FD_ISSET(this->masterSocket, &this->readfds)) {
         AcceptConnection();
      }

      for (auto it = this->clientFds.begin(); it != this->clientFds.end();
           it++) {
         int clientFd = *it;

         if (FD_ISSET(clientFd, &this->readfds)) {
            HandleRequest(clientFd);
         }
      }
   }

   return;
}


int
Server::PrepareSelect()
{
   int maxFd = this->masterSocket;

   FD_ZERO(&this->readfds);
   FD_ZERO(&this->writefds);
   FD_SET(this->masterSocket, &this->readfds);
   for(auto it = this->clientFds.begin(); it != this->clientFds.end(); it++) {
      int clientFd = *it;

      FD_SET(clientFd, &this->readfds);
      if (clientFd > maxFd) {
         maxFd = clientFd;
      }
   }

   return maxFd;
}

void
Server::AcceptConnection()
{
   struct sockaddr_in address = {0};
   int addrlen = 0;
   int newClientFd = 0;
   pair<set<int>::iterator, bool> ret;

   if ((newClientFd = accept(this->masterSocket,
                             (struct sockaddr *)&address,
                             (socklen_t*)&addrlen))<0) {
      throw ("Fail to accept");
   }

   ret = this->clientFds.insert(newClientFd);
   /*
    * The newly created client fd should be unique.
    */
   assert(ret.second);
   printf("Accepted new connection %d\n", newClientFd);
   return;
}

void
Server::HandleRequest(int clientFd)
{
   char *buffer = new char[MAX_MSG_BUFFER_SIZE];
   size_t bytes;

   if ((bytes = read(clientFd, buffer, MAX_MSG_BUFFER_SIZE)) > 0) {
      Request request(clientFd, buffer, bytes);
      HandleRequestInt(request);
   } else {
      close(clientFd);
      this->clientFds.erase(clientFd);
   }

   delete buffer;
   return;
}

