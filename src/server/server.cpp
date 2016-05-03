/*
 * @file server.cpp
 * @brief Contains the implementation of the Server base class.
 * @author yihuaf
 */

#include "server.h"
#include <cassert>
#include <cstring>

using namespace std;

Server::Request::Request()
   :clientFd(0),
    numBytes(0)
{}

inline char *
Server::Request::GetBufferForRecv()
{
   return this->recvBuffer + this->numBytes;
}

inline size_t
Server::Request::GetSizeForRecv()
{
   return MAX_MSG_BUFFER_SIZE - this->numBytes;
}

Server::Server(int port)
:port(port)
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
   if(::bind(this->masterSocket, (struct sockaddr *)&address,
             sizeof(address))) {
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
   for (;;) {
      int maxFd = PrepareSelect();
      int numReady = select(maxFd + 1 , &this->readfds , &this->writefds, NULL,
                            NULL);
      if (numReady < 0 && errno != EINTR) {
         perror("Select error");
         continue;
      }

      if (FD_ISSET(this->masterSocket, &this->readfds)) {
         AcceptConnection();
      }

      for(const auto &context : this->clientContext) {
         int clientFd = context.first;

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
   for(const auto &context : this->clientContext) {
      int clientFd = context.first;
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

   if ((newClientFd = accept(this->masterSocket,
                             (struct sockaddr *)&address,
                             (socklen_t*)&addrlen))<0) {
      throw ("Fail to accept");
   }

   /*
    * The newly created client fd should be unique.
    */
   assert(this->clientContext.find(newClientFd) == this->clientContext.end());
   this->clientContext.insert(pair<int, Request *>(newClientFd, NULL));
   printf("Accepted new connection %d\n", newClientFd);
   return;
}

void
Server::HandleRequest(int clientFd)
{
   map<int, Request *>::iterator it = this->clientContext.find(clientFd);
   assert(it != this->clientContext.end());
   if (it->second == NULL) {
      it->second = new Request();
   }

   Request *request = it->second;
   int bytes = read(clientFd, request->GetBufferForRecv(),
                    request->GetSizeForRecv());
   if (bytes > 0) {
      if (HandleRequestInt(request)) {
         delete request;
         it->second = NULL;
      }
   } else {
      close(clientFd);
      delete request;
      this->clientContext.erase(it);
      if (bytes < 0) {
         throw ("Failed to receive");
      }
   }

   return;
}

