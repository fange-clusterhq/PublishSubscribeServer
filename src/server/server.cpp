/*
 * @file server.cpp
 * @brief Contains the implementation of the Server base class.
 * @author yihuaf
 */

#include "server.h"
#include <cassert>
#include <cstring>

using namespace std;

Request::Request()
   :clientFd(0),
    numBytes(0)
{
   memset(this->buffer, 0, MAX_MSG_BUFFER_SIZE);
}

ReadRequest::ReadRequest()
   :Request()
{}

inline char *
ReadRequest::GetBufferForRecv()
{
   /* Taken into account the the buffer may contain a partial message. */
   return this->buffer + this->numBytes;
}

inline size_t
ReadRequest::GetSizeForRecv()
{
   /* Taken into account the the buffer may contain a partial message. */
   return MAX_MSG_BUFFER_SIZE - this->numBytes;
}

void
ReadRequest::Consume(size_t bytes)
{
   assert(bytes <= this->numBytes);

   if (bytes == 0 || bytes > this->numBytes) {
      return;
   }

   memcpy(this->buffer, this->buffer + bytes, this->numBytes - bytes);
   this->numBytes -= bytes;
   /* NULL terminates the buffer so we can use string lib. */
   this->buffer[this->numBytes] = '\0';
   return;
}

WriteRequest::WriteRequest()
   :Request()
{}

Server::Server(int port)
:port(port)
{}

Server::~Server() {}

void
Server::Init()
{
   signal(SIGPIPE, SIG_IGN);
   struct sockaddr_in address;

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

      /* Handle new connection. */
      if (FD_ISSET(this->masterSocket, &this->readfds)) {
         AcceptConnection();
      }

      /* Handle incoming requests. */
      for(auto it = this->clientContext.begin();
          it != this->clientContext.end();) {
         int clientFd = it->first;
         it++;
         if (FD_ISSET(clientFd, &this->readfds)) {
            HandleRequest(clientFd);
         }
      }

      /* Handle outgoing messages/responses. */
      for(auto it = this->msgOutgoingQueue.begin();
          it != this->msgOutgoingQueue.end();) {
         int clientFd = it->first;
         it++;
         if (FD_ISSET(clientFd, &this->writefds)) {
            HandleOutgoingMsg(clientFd);
         }
      }
   }

   return;
}


int
Server::PrepareSelect()
{
   int maxFd = this->masterSocket;
   /*
    * Adding all accepted connection to readfds and all fd that has pending
    * messages in queue to the writefds.
    */
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

   for(const auto &outgoingQueue : this->msgOutgoingQueue) {
      int clientFd = outgoingQueue.first;
      FD_SET(clientFd, &writefds);
      if (clientFd > maxFd) {
         maxFd = clientFd;
      }
   }

   return maxFd;
}

void
Server::AcceptConnection()
{
   struct sockaddr_in address;
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
   this->clientContext[newClientFd] =  NULL;
   printf("Accepted new connection %d\n", newClientFd);
   return;
}


void
Server::HandleRequest(int clientFd)
{
   auto it = this->clientContext.find(clientFd);
   assert(it != this->clientContext.end());
   /*
    * It is possible that the last receive from this client only obtained
    * a particial request. We would stored this particial request in the
    * clientContext. If we did not have a particial request, then we
    * create a new ReadRequest in the context.
    */
   if (it->second == NULL) {
      it->second = new ReadRequest();
   }

   ReadRequest *request = it->second;
   ssize_t bytes = read(clientFd, request->GetBufferForRecv(),
                        request->GetSizeForRecv());
   if (bytes > 0) {
      request->clientFd = clientFd;
      request->numBytes += bytes;
      if (HandleRequestInt(request)) {
         /*
          * If HandleRequestInt tells us that the request received is complete
          * or not a particial request, we can safely delete the request. Else,
          * as the request is already stored in the context, we need to do
          * nothing.
          */
         delete request;
         it->second = NULL;
      }
   } else {
      printf("Connection Closed\n");
      close(clientFd);
      delete request;
      this->clientContext.erase(it);
      /*
       * Since the connection has closed, all pending msg to this fd should be
       * dropped.
       */
      this->msgOutgoingQueue.erase(clientFd);
      if (bytes < 0) {
         perror("Failed to receive");
      }
   }

   return;
}


bool
Server::HandleRequestInt(ReadRequest *request)
{
   WriteRequest *newRequest = new WriteRequest();
   /* By default, we echo back what we received. */
   newRequest->clientFd = request->clientFd;
   memcpy(newRequest->buffer, request->buffer, request->numBytes);
   newRequest->numBytes = request->numBytes;
   this->queueMsg(request->clientFd, newRequest);
   /* Echos back, so it will never be partial. */
   return true;
}


void
Server::queueMsg(int clientFd, WriteRequest *request)
{
   msgOutgoingQueue[clientFd].push(request);
   return;
}


WriteRequest *
Server::dequeueMsg(int clientFd)
{
   map<int, queue<WriteRequest *>>::iterator it =
      this->msgOutgoingQueue.find(clientFd);
   assert(it != this->msgOutgoingQueue.end());
   queue<WriteRequest *> &msgQueue = it->second;
   /*
    * We should not have an empty queue since we erased all the empty queue.
    */
   assert(!msgQueue.empty());
   WriteRequest *request = msgQueue.front();
   msgQueue.pop();
   if (msgQueue.empty()) {
      msgOutgoingQueue.erase(clientFd);
   }

   return request;
}


void
Server::HandleOutgoingMsg(int clientFd)
{
   WriteRequest *request = dequeueMsg(clientFd);
   assert(request != NULL);
   int numBytesExpectedSend = request->numBytes;
   int numBytesActualSend = 0;
   ssize_t ret;
   while(numBytesActualSend < numBytesExpectedSend) {
      ret = send(clientFd, request->buffer,
                 request->numBytes - numBytesActualSend, 0);
      if (ret < 0) {
         /*
          * We simply drop the write request in the case there is a write
          * error.
          */
         perror("Failed to send");
         break;
      }

      numBytesActualSend += ret;
   }

   delete request;
   return;
}
