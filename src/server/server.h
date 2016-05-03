/*
 * @file server/server.h
 * @brief Definitions for the server base class.
 *
 * This contains the definitions for a server base class that can be
 * used for all other types of server implementation.
 *
 * @author Yihua Eric Fang (yihuaf)
 */

#pragma once

#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <queue>

using namespace std;

static const int MAX_NUM_CONNECTION = 64;
/* Ideally, a HTTP header should not exceeds this length. */
static const int MAX_MSG_BUFFER_SIZE = 4096;

class Server {
   public:
      /* @brief Constructor.
       *
       * @param port The port this server will listen to.
       * @return None.
       */
      Server(int port);
      /* @brief Destructor.
       *
       * @return None.
       */
      ~Server();

      /* @brief Initialize the server.
       *
       * We choose not to initialize everything in the Constructor because
       * we want to avoid throwing exceptions in the case of an error inside
       * a constructor.
       *
       * @return None.
       */
      void Init();
      /* @brief Start the server and serving forever.
       *
       * @return None.
       */
      void Start();
   protected:
      class Request {
         public:
            /* This request is received from thsi client. */
            int clientFd;
            char recvBuffer[MAX_MSG_BUFFER_SIZE];
            size_t numBytes;

            Request();
            /* @brief Return a pointer to the buffer for receiving new data.
             *
             * The pointer returned is the position of where the current buffer
             * is. There might be data stored from the last receive and is only
             * a particial valid request.
             *
             * @return None.
             */
            inline char *GetBufferForRecv();
            /* @brief Return the size available in the buffer.
             *
             * As mentioned before, the buffer may have stored some data
             * already, so we return the size currently available.
             *
             * @return None.
             */
            inline size_t GetSizeForRecv();
      };

   private:
      /* Listening to this port. */
      int port;
      /* Fd sets for the select(). */
      fd_set readfds;
      fd_set writefds;
      /* Socket used for the server to accept new connections. */
      int masterSocket;
      /*
       * Store the client information for each of the client. The clientFd
       * should be present in the map if the connection is not closed. The
       * request should not be NULL us previously this client sent a partial
       * request.
       */
      map<int, Request *> clientContext;
      /*
       * Store all the pending out going messages.
       */
      map<int, queue<Request *>> msgOutgoingQueue;

      /* @brief Accept a new connection.
       *
       * To accept a new connection, we store the fd into the clientContext.
       *
       * @return None.
       */
      void AcceptConnection();

      /* @brief Handle an incoming request.
       *
       * When a new fd is ready for read, this method is called to received the
       * data and process it. If the request cannot be processed, it will store
       * tha partial request into the clientContent.
       *
       * @return None.
       */
      void HandleRequest(int clientFd);

      /* @brief Prepare the fd sets for the select().
       *
       * The select() function require the fd sets to be reinitialized each
       * time it is being called. Therefore, we need to put all the fds into
       * the set.
       *
       * @return None.
       */
      int PrepareSelect();

      /* @brief Handle the request after the data is received.
       *
       * The function should do the heavy lifting to process the data received
       * and determine if the current request is full or partial.  The derived
       * class of this class should override this method.  For this class, this
       * method will echo the received back to sender.
       *
       * @params request The request containing the data received.
       * @return Is this request done or only a partial.
       */
      bool HandleRequestInt(Request *request);

      /* @brief Queue the msg to the queue.
       *
       * We first queue the messages to the queue. Then, the destnation fd will
       * be set for the select(). When the dest fd becomes available to write
       * the server will sent them out.
       *
       * @params clientFd The dest client fd.
       * @params request Contains msg to be sent out.
       * @return None.
       */
      void queueMsg(int clientFd, Request *request);

      Request *dequeueMsg(int clientFd);
      void HandleOutgoingMsg(int clientFd);
};
