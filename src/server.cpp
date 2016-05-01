/*
 * @file server.cpp
 * @brief Contains the implementation of the Publish Subscribe Server.
 * @author yihuaf
 */

#include "server.h"
#include <cassert>
#include <cstring>

Server::Server(int port)
:port(port),
 clientFds()
{
}

Server::~Server() {}

void
Server::Init()
{
   struct sockaddr_in address = {0};

   if((this->masterSocket = socket(AF_INET, SOCK_STREAM ,0)) == 0) {
      perror("Failed to initiate the master socket");
      exit(EXIT_FAILURE);
   }

   address.sin_family = AF_INET;
   address.sin_addr.s_addr = INADDR_ANY;
   address.sin_port = htons(port);
   if (bind(this->masterSocket, (struct sockaddr *)&address,
            sizeof(address)) < 0) {
      perror("Fail to bind");
      exit(EXIT_FAILURE);
   }

   if (listen(this->masterSocket, MAX_NUM_CONNECTION) < 0) {
      perror("Fail to listen");
      exit(EXIT_FAILURE);
   }
}

void
Server::Start()
{
   int maxFd;
   int numReady;

   while(true) {
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
   std::pair<std::set<int>::iterator, bool> ret;

   if ((newClientFd = accept(this->masterSocket,
                             (struct sockaddr *)&address,
                             (socklen_t*)&addrlen))<0) {
      perror("Fail to accept");
      exit(EXIT_FAILURE);
   }

   ret = this->clientFds.insert(newClientFd);
   assert(ret.second);
   printf("Accepted new connection %d\n", newClientFd);
   return;
}

void
Server::HandleRequest(int clientFd)
{
   char buffer[MAX_MSG_BUFFER_SIZE];
   int byte;

   if ((byte = read(clientFd, buffer, MAX_MSG_BUFFER_SIZE)) > 0) {
      int ret;

      // FIXME: change this.
      buffer[byte] = '\0';
      ret = send(clientFd, buffer, strlen(buffer), 0);
      if (ret < 0 || ret != byte) {
         perror("Fail to send data");
         exit(EXIT_FAILURE);
      }
   } else {
      close(clientFd);
      this->clientFds.erase(clientFd);
   }

   return;
}

